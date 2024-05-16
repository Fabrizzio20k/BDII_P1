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
        T data;
        int next {0};
        friend SequentialFile;
    public:
        Record(T& data) : data(data) {}
    };

    static const int blockSize = (1<<12); // block size in bytes
    static const int blockingFactor = blockSize / sizeof(Record);

    int N {blockingFactor}; // number of records in first block to manage in memory
    Mode mode {SEARCH};

    int n {}; // number of records in main space
    int K {}; // maximum number of records in auxiliary space
    int k {}; // current number of records in auxiliary space
    int delCnt {}; // number of removed records
    int start {}; // position of first element (ordered)

    function<Key(T&)> getKey;

    char* filename;

    class Block {
        Record records[SequentialFile::blockingFactor];
        friend SequentialFile;
    public:
        Block() = default;
    };

    void setK() {
        switch (mode) {
            case SEARCH: K = 64 - __builtin_clzll(blockingFactor); break;
            case INSERT:
                double x {}, m {static_cast<double>(N)};
                for (double k1 = m; k1 > 0; k1/=2) {
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
        f.write(reinterpret_cast<char*>(&K), sizeof(int));
        f.write(reinterpret_cast<char*>(&n), sizeof(int));
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
        for (int x = n - 1; x > 0; x >>= 1) {
            while (pos + x <= n) {
                record = getRecord(pos + x);
                if (getKey(record.data) <= key) pos += x;
                else break;
            }
        }
        while (pos && record.next == -1) record = getRecord(--pos);
        return {record, pos};
    }

    bool mainSpaceInsert(Record record) {
        Key key = getKey(record.data);

        Block block = getFirstBlock();
        auto cmp = [&](Record& r1, Record& r2)->bool {
            return getKey(r1.data) < getKey(r2.data);
        };
        Record match = lower_bound(block.records, block.records + n, record, cmp);
        if (getKey(match.data) == key) return false;

        block.records[n++] = record;
        sort(block.records, block.records + n, cmp);
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
            while (getKey(match.data) <= key) {
                pos = match.next;
                match = getRecord(pos);
            }

            if (getKey(match.data) == key) return false;
            if (getKey(match.data) > key) {
                record.next = start;
                start = n + k + 1;
            }
            else {
                record.next = match.next;
                match.next = n + k + 1;
                setRecord(match, pos);
            }
        }
        else if (getKey(match.data) == key) return false;
        else {
            int nextPos = match.next;
            Record next {};
            while (nextPos) {
                next = getRecord(nextPos);
                if (getKey(next) > key) break;

                match = next;
                pos = nextPos;
                nextPos = match.next;
            }

            if (getKey(match.data) == key) return false;

            match.next = n + k + 1;
            record.next = nextPos;
            setRecord(match, pos);
        }

        updateHeader();
        setRecord(record, ++k + n);

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
        setK();
        setHeader();

        for (; ind < n; ++ind) {
            records[ind].next = ind + 2;
            setRecord(records[ind], ind + 1);
        }
    }

public:
    SequentialFile(const char* filename, Mode mode, function<Key(T&)> getKey) : mode(mode), getKey(getKey) {
        strcpy(this->filename, filename);

        fstream f (filename, ios::in | ios::out | ios::binary);
        bool exists = f.is_open();
        f.close();

        if (exists) processHeader();
        else {
            setK(mode);
            setHeader();
            Block block {};
            setFirstBlock(block);
        }
    }

    pair<T,bool> search(Key key) {
        auto [record, pos] = _search(key);
        if (getKey(record.data) == key) return {record, true};

        pos = n + 1;
        while (pos <= n + k) {
            record = getRecord(pos++);
            if (record.next == -1) continue;
            if (getKey(record) == key) return {record, true};
        }
        return {Record{}, false};
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
        for (int x = n - 1; x > 0; x >>= 1) {
            while (pos + x <= n) {
                record = getRecord(pos + x);
                if (getKey(record.data) < key) pos += x;
                else break;
            }
        }
        while (pos && record.next == -1) record = getRecord(--pos);

        if (pos == 0) {
            pos = start;
            record = getRecord(pos);
        }

        Record next {};
        while (record.next) {
            next = getRecord(record.next);
            if (getKey(next.data) >= key) break;

            pos = record.next;
            record = next;
        }
        if (record.next) {
            if (getKey(next) != key) return false;

            int nextPos = record.next;

            record.next = next.next;
            next.next = -1;
            setRecord(record, pos);
            setRecord(next, nextPos);

            ++delCnt;
            updateHeader();
            return true;
        }
        else if (getKey(record) == key) {
            start = record.next;
            record.next = -1;
            setRecord(record, pos);

            ++delCnt;
            updateHeader();
            return true;
        }
        else return false;
    }

    vector<T> range(Key left, Key right) {

    }
};

#endif //DB2_SEQUENTIAL_FILE_SEQUENTIALFILE_H
