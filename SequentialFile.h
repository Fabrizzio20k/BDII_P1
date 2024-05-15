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
        size_t next {0};
    public:
        Record(T& data) : data(data) {}
    };

    void setK(Mode mode) {
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
                K = static_cast<size_t>(x);
        }
    }

    void setHeader() {
        ofstream f (filename, ios::binary);
        f.write(reinterpret_cast<char*>(&N), sizeof(size_t));
        f.write(reinterpret_cast<char*>(&K), sizeof(size_t));
        f.write(reinterpret_cast<char*>(&n), sizeof(size_t));
        f.write(reinterpret_cast<char*>(&k), sizeof(Record) - 3 * sizeof(size_t));
        f.close();
    }

    void processHeader() {
        ifstream f (filename, ios::binary);
        f.read(reinterpret_cast<char*>(&N), sizeof(size_t));
        f.read(reinterpret_cast<char*>(&K), sizeof(size_t));
        f.read(reinterpret_cast<char*>(&n), sizeof(size_t));
        f.read(reinterpret_cast<char*>(&k), sizeof(size_t));
        f.close();
    }

    void updateHeader() {
        fstream f (filename, ios::in | ios::out | ios::binary);
        f.write(reinterpret_cast<char*>(&N), sizeof(size_t));
        f.write(reinterpret_cast<char*>(&K), sizeof(size_t));
        f.write(reinterpret_cast<char*>(&n), sizeof(size_t));
        f.write(reinterpret_cast<char*>(&k), sizeof(Record) - 3 * sizeof(size_t));
        f.close();
    }

    static const size_t blockSize = (1<<12); // block size in bytes
    static const size_t blockingFactor = blockSize / sizeof(Record);
    size_t N {blockingFactor};
    size_t K {};

    size_t n {};
    size_t k {};

    function<bool(T&, T&)> cmp;

    char* filename;

public:
    SequentialFile(const char* filename, Mode mode, function<bool(T&, T&)> cmp) : cmp(cmp) {
        strcpy(this->filename, filename);

        fstream f (filename, ios::in | ios::out | ios::binary);
        bool exists = f.is_open();
        f.close();

        if (exists) processHeader();
        else {
            setK(mode);
            setHeader();
        }
    }
    pair<T,bool> search(Key key) {

    }
    bool insert(T data) {

    }
    bool remove(Key key) {

    }
    vector<T> range(Key left, Key right) {

    }
};

#endif //DB2_SEQUENTIAL_FILE_SEQUENTIALFILE_H
