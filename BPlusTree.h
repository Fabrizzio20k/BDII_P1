//
// Created by iansg on 5/17/2024.
//

#ifndef DB2_BPLUSTREE_BPLUSTREE_H
#define DB2_BPLUSTREE_BPLUSTREE_H

#include <iostream>
#include <functional>
#include <vector>
using namespace std;

// clustered B+ tree
template<typename Record, typename Key, int keySize>
class BPlusTree {
    static const int blockSize = (1<<8); // block size in bytes

    // m * 2 * sizeof(int) + (m * 2 - 1) * sizeof(Key) + sizeof(int) <= blockSize
    // m * 2 * (sizeof(int) + sizeof(Key)) = blockSize - sizeof(int) - sizeof(Key)
    // m = (blockSize - sizeof(int) - sizeof(Key) / (2 * (sizeof(int) + sizeof(Key))
    static const int m { (blockSize - sizeof(int) - keySize) / (2 * (sizeof(int) + keySize)) }; // m * 2 = number of children | m * 2 - 1 = number of keys
    static const int blockingFactor = (blockSize - sizeof(int)) / sizeof(Record);

    function<Key(Record&)> getKey; // function to extract key from record
    function<int(Key,Key)> cmp; // function to compare two keys: -1 ~= < | 0 ~= == | 1 ~= >

    int root {1}; // position of first element (ordered)

    char filename[50];

    class Node {
        int n {};
        Key keys[(m << 1) - 1];
        int children[(m << 1)];
        friend BPlusTree;
    public:
        Node() = default;
    };

    class Leaf {
        int n {};
        Record records[blockingFactor];
        friend BPlusTree;
    public:
        Leaf() = default;
    };


public:
    BPlusTree() = default;
    ~BPlusTree() = default;
    pair<Record,bool> search(Key key) {

    }
    bool insert(Record record) {

    }
    bool remove(Key key) {

    }
    vector<Record> range(Key left, Key right) {

    }
};

#endif //DB2_BPLUSTREE_BPLUSTREE_H
