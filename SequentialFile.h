//
// Created by iansg on 5/13/2024.
//

#ifndef DB2_SEQUENTIAL_FILE_SEQUENTIALFILE_H
#define DB2_SEQUENTIAL_FILE_SEQUENTIALFILE_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <functional>
using namespace std;

template<typename T, typename Key>
class SequentialFile {
    enum Mode { INSERT, SEARCH};

    class Record {
        T data {};
        int next {0};
        friend SequentialFile;
    public:
        Record() = default;
        explicit Record(T& data) : data(data) {}
    };

    static const int blockSize = (1<<8); // block size in bytes
    static const int blockingFactor = blockSize / sizeof(Record);

    int N {blockingFactor}; // number of records in first block to manage in memory
    Mode mode {SEARCH};

    int n {}; // number of records in main space
    int K {}; // maximum number of records in auxiliary space
    int k {}; // current number of records in auxiliary space
    int delCnt {}; // number of removed records
    int start {1}; // position of first element (ordered)

    function<Key(T&)> getKey; // function to extract key from data
    function<int(Key,Key)> cmp; // function to compare two keys: -1 ~= < | 0 ~= == | 1 ~= >

    char filename[50];

    class Block {
        Record records[SequentialFile::blockingFactor] {};
        friend SequentialFile;
    public:
        Block() = default;
    };

    void setK() {
        switch (mode) {
            case SEARCH: K = 32 - __builtin_clz((n?n:N)); break;
            case INSERT:
                double x {}, m {static_cast<double>(N)};
                for (double k1 = m; k1 > 0; k1 /= 2.0) {
                    double p = (x + k1) * (x + k1);
                    while (p < m && m - p < 1e-9) {
                        x+=k1;
                        p = (x + k1) * (x + k1);
                    }
                }
                K = static_cast<int>(x);
        }
    }

    void setHeader() {
        ofstream f (filename, ios::binary);
        f.write(reinterpret_cast<char*>(&n), sizeof(int));
        f.write(reinterpret_cast<char*>(&K), sizeof(int));
        f.write(reinterpret_cast<char*>(&k), sizeof(int));
        f.write(reinterpret_cast<char*>(&delCnt), sizeof(int));
        f.write(reinterpret_cast<char*>(&start), sizeof(Record) - 4 * sizeof(int));
        f.close();
    }

    void processHeader() {
        ifstream f (filename, ios::binary);
        f.read(reinterpret_cast<char*>(&n), sizeof(int));
        f.read(reinterpret_cast<char*>(&K), sizeof(int));
        f.read(reinterpret_cast<char*>(&k), sizeof(int));
        f.read(reinterpret_cast<char*>(&delCnt), sizeof(int));
        f.read(reinterpret_cast<char*>(&start), sizeof(int));
        f.close();
    }

    void updateHeader() {
        fstream f (filename, ios::in | ios::out | ios::binary);
        f.write(reinterpret_cast<char*>(&n), sizeof(int));
        f.write(reinterpret_cast<char*>(&K), sizeof(int));
        f.write(reinterpret_cast<char*>(&k), sizeof(int));
        f.write(reinterpret_cast<char*>(&delCnt), sizeof(int));
        f.write(reinterpret_cast<char*>(&start), sizeof(Record) - 4 * sizeof(int));
        f.close();
    }

    Block getFirstBlock() {
        ifstream f (filename, ios::binary);
        f.seekg(sizeof(Record));

        Block block {};
        f.read(reinterpret_cast<char*>(&block), sizeof(Block));

        f.close();
        return block;
    }

    void setFirstBlock(Block& block) {
        fstream f (filename, ios::in | ios::out | ios::binary);
        f.seekp(sizeof(Record));

        f.write(reinterpret_cast<char*>(&block), sizeof(Block));

        f.close();
    }

    Record getRecord(int pos) {
        ifstream f (filename, ios::binary);
        f.seekg(pos * sizeof(Record));

        Record record;
        f.read(reinterpret_cast<char*>(&record), sizeof(Record));

        f.close();
        return record;
    }

    void setRecord(Record& record, int pos) {
        fstream f (filename, ios::in | ios::out | ios::binary);
        f.seekp(pos * sizeof(Record));
        f.write(reinterpret_cast<char*>(&record), sizeof(Record));
        f.close();
    }

    pair<Record,int> _search(Key key) {
        int pos {0};
        Record record {};
        for (int x = n; x > 0; x >>= 1) {
            while (pos + x <= n) {
                record = getRecord(pos + x);
                if (cmp(getKey(record.data), key) < 1) pos += x;
                else break;
            }
        }
        if (pos) record = getRecord(pos);
        while (pos && record.next == -1) record = getRecord(--pos);
        return {record, pos};
    }

    bool mainSpaceInsert(Record record) {
        Key key = getKey(record.data);

        Block block = getFirstBlock();
        auto recordCmp = [&](Record& r1, Record& r2)->bool {
            return cmp(getKey(r1.data), getKey(r2.data)) == 0;
        };

        int pos {1};
        for (int x = n - 1; x > 0; x >>= 1)
            while (pos + x <= n) {
                Record match = getRecord(pos + x);
                if (cmp(getKey(match.data), key) == 0) return false;
                if (cmp(getKey(match.data), key) == 1) break;
                pos += x;
            }

        block.records[n++] = record;
        sort(block.records, block.records + n, recordCmp);
        for (int i = 1; i < n; ++i)
            block.records[i - 1].next = i + 1;

        updateHeader();
        setFirstBlock(block);

        return true;
    }

    bool auxiliarySpaceInsert(Record record) {
        Key key = getKey(record.data);
        auto [match, pos] = _search(key);

        if (pos == 0) {
            pos = start;
            match = getRecord(pos);
            while (cmp(getKey(match.data), key) < 1) {
                pos = match.next;
                match = getRecord(pos);
            }

            if (cmp(getKey(match.data), key) == 0) return false;
            if (cmp(getKey(match.data), key) == 1) {
                record.next = start;
                start = n + k + 1;
            }
            else {
                record.next = match.next;
                match.next = n + k + 1;
                setRecord(match, pos);
            }
        }
        else if (cmp(getKey(match.data), key) == 0) return false;
        else {
            int nextPos = match.next;
            Record next {};
            while (nextPos) {
                next = getRecord(nextPos);
                if (cmp(getKey(next.data), key) == 1) break;

                match = next;
                pos = nextPos;
                nextPos = match.next;
            }

            if (cmp(getKey(match.data), key) == 0) return false;

            match.next = n + k + 1;
            record.next = nextPos;
            setRecord(match, pos);
        }

        setRecord(record, ++k + n);
        updateHeader();

        return true;
    }

    void rebuild() {
        int it {start}, ind {};
        Record records [n + k - delCnt];
        while (it) {
            records[ind] = getRecord(it);
            it = records[ind++].next;
        }

        ind = 0;
        n = n + k - delCnt;
        k = 0;
        delCnt = 0;
        start = 1;
        setK();
        setHeader();

        for (; ind < n - 1; ++ind) {
            records[ind].next = ind + 2;
            setRecord(records[ind], ind + 1);
        }
        records[n - 1].next = 0;
        setRecord(records[n - 1], n);
    }

public:
    SequentialFile(const char* filename, function<Key(T&)> getKey, function<int(Key,Key)> cmp, bool searchMode=true) : getKey(getKey), cmp(cmp) {
        strcpy(this->filename, filename);

        if (!searchMode) mode = INSERT;

        fstream f (filename, ios::in | ios::out | ios::binary);
        bool exists = f.is_open();
        f.close();

        if (exists) processHeader();
        else {
            setK();
            setHeader();
            Block block {};
            setFirstBlock(block);
        }
    }
    ~SequentialFile() = default;

    pair<T,bool> search(Key key) {
        auto [record, pos] = _search(key);
        if (cmp(getKey(record.data), key) == 0) return {record.data, true};

        pos = n + 1;
        while (pos <= n + k) {
            record = getRecord(pos++);
            if (record.next == -1) continue;
            if (cmp(getKey(record.data), key) == 0) return {record.data, true};
        }
        return {T{}, false};
    }

    bool insert(T data) {
        Record record = Record(data);
        if (n < N) return mainSpaceInsert(record);
        if (k == K) rebuild();
        return auxiliarySpaceInsert(record);
    }

    bool remove(Key key) {
        if (n == 0) return false;
        if (delCnt == K) rebuild();

        int pos {0};
        Record record {};
        for (int x = n; x > 0; x >>= 1) {
            while (pos + x <= n) {
                record = getRecord(pos + x);
                if (cmp(getKey(record.data), key) == -1) pos += x;
                else break;
            }
        }
        while (pos) {
            record = getRecord(pos);
            if (record.next >= 0) break;
            --pos;
        }

        if (pos == 0) pos = start;
        record = getRecord(pos);

        Record next {};
        while (record.next) {
            next = getRecord(record.next);
            if (cmp(getKey(next.data), key) >= 0) break;

            pos = record.next;
            record = next;
        }
        if (cmp(getKey(record.data), key) == 0) {
            start = record.next;
            record.next = -1;
            setRecord(record, pos);

            ++delCnt;
            updateHeader();
            return true;
        }
        else if (record.next) {
            if (cmp(getKey(next.data), key) != 0) return false;

            int nextPos = record.next;

            record.next = next.next;
            next.next = -1;
            setRecord(record, pos);
            setRecord(next, nextPos);

            ++delCnt;
            updateHeader();
            return true;
        }
        else return false;
    }

    vector<T> range(Key left, Key right) {
        auto [record, pos] = _search(left);

        if (pos == 0 || cmp(getKey(record.data), left) < 0) {
            pos = start;
            record = getRecord(start);
            while (pos) {
                if (cmp(getKey(record.data), left) >= 0) break;
                pos = record.next;
            }
        }

        vector<T> result;
        while (pos) {
            if (cmp(getKey(record.data), right) == 1) break;
            result.push_back(record.data);
            pos = record.next;
            record = getRecord(pos);
        }
        return result;
    }

    void print() {
        Record record {};
        cout << "=========MAIN SPACE============\n";
        for (int i = 1; i <= n; ++i) {
            record = getRecord(i);
            cout << record.data;
        }

        cout << "=======AUXILIARY SPACE==========\n";
        for (int i = 1; i <= k; ++i) {
            record = getRecord(n + i);
            cout << record.data;
        }
    }
};

#endif //DB2_SEQUENTIAL_FILE_SEQUENTIALFILE_H
