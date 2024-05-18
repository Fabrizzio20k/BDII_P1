//
// Created by iansg on 5/17/2024.
//

#ifndef DB2_BPLUSTREE_BPLUSTREE_H
#define DB2_BPLUSTREE_BPLUSTREE_H

#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <cstring>
using namespace std;

// clustered B+ tree
template<typename Record, typename Key, int keySize>
class BPlusTree {
    static const int blockSize = (1<<8); // block size in bytes

    // t * 2 * sizeof(int) + (t * 2 - 1) * sizeof(Key) + sizeof(int) + sizeof(bool) <= blockSize
    // t * 2 * (sizeof(int) + sizeof(Key)) = blockSize - sizeof(int) - sizeof(bool) - sizeof(Key)
    // t = (blockSize - sizeof(bool) - sizeof(int) - sizeof(Key) / (2 * (sizeof(int) + sizeof(Key))
    static const int t { // t = minimum degree
        (blockSize - sizeof(bool) - sizeof(int) - keySize) / (2 * (sizeof(int) + keySize))
    };
    static const int blockingFactor = (blockSize - 3 * sizeof(int)) / sizeof(Record);

    function<Key(Record&)> getKey; // function to extract key from record
    function<int(Key,Key)> cmp; // function to compare two keys: -1 ~= < | 0 ~= == | 1 ~= >
    function<bool(Record&,Record&)> recordCmp; // function to compare two keys: -1 ~= < | 0 ~= == | 1 ~= >

    int root {}; // pointer to root
    int lastDel {}; // pointer to last deleted block
    int headerSize {sizeof(root) + sizeof(lastDel)};

    char filename[50];

    class Node {
        int n {};
        Key k[(t << 1) - 1];
        int c[(t << 1)];
        bool leaf {true};
        friend BPlusTree;
    public:
        Node() = default;
    };

    class Leaf {
        int n {};
        int next {};
        int nextDel {-1};
        Record records[blockingFactor];
        friend BPlusTree;
    public:
        Leaf() = default;
        explicit Leaf(int n) : n(n) {}
    };

    int getPtr(int pos) { return headerSize + pos * blockSize; }
    int getLastPos(int end)  { return (end - headerSize) / blockSize; }

    int allocatePos() {
        int pos {};
        if (lastDel) {
            Leaf leaf = getLeaf(lastDel);
            pos = lastDel;
            lastDel = leaf.nextDel;

            setHeader();
            return pos;
        }
        else {
            fstream f (filename, ios::in | ios::out | ios::binary);
            f.seekg(ios::end);
            pos = getLastPos(f.tellg());
            f.close();
            return pos;
        }
    }

    void createHeader() {
        ofstream f (filename, ios::binary);
        f.write(reinterpret_cast<char*>(&root), sizeof(root));
        f.write(reinterpret_cast<char*>(&lastDel), sizeof(lastDel));
        f.close();
    }

    void setHeader() {
        fstream f (filename, ios::in | ios::out | ios::binary);
        f.write(reinterpret_cast<char*>(&root), sizeof(root));
        f.write(reinterpret_cast<char*>(&lastDel), sizeof(lastDel));
        f.close();
    }

    void getHeader() {
        ifstream f (filename, ios::binary);
        f.read(reinterpret_cast<char*>(&root), sizeof(root));
        f.read(reinterpret_cast<char*>(&lastDel), sizeof(lastDel));
        f.close();
    }

    void setLeaf(Leaf& leaf, int pos) {
        fstream f (filename, ios::in | ios::out | ios::binary);
        f.seekp(getPtr(pos));
        f.write(reinterpret_cast<char*>(&leaf), sizeof(blockSize));
        f.close();
    }

    Leaf getLeaf(int pos) {
        ifstream f (filename, ios::binary);
        f.seekg(getPtr(pos));

        Leaf leaf {};
        f.read(reinterpret_cast<char*>(&leaf), sizeof(Leaf));

        f.close();
        return leaf;
    }

    void setNode(Node& node, int pos) {
        fstream f (filename, ios::in | ios::out | ios::binary);
        f.seekp(getPtr(pos));
        f.write(reinterpret_cast<char*>(&node), blockSize);
        f.close();
    }

    Node getNode(int pos) {
        ifstream f (filename, ios::binary);

        Node node {};
        f.seekg(getPtr(pos));
        f.read(reinterpret_cast<char*>(&node), sizeof(Node));

        f.close();
        return node;
    }

    Node _splitChild(Node x, int pos, int i) {
        Node y = getNode(x.c[i]);
        Node z {};
        // move y's last t-1 keys to z
        for (int j = 0; j < t - 1; ++j)
            z.k[j] = y.k[j + t];
        // move y's last t children to z
        for (int j = 0; j < t; ++j)
            z.c[j] = y.c[j + t];
        // update y's size
        y.n = t - 1;
        // update z's size & leaf attributes
        z.n = t - 1;
        z.leaf = y.leaf;
        // shift x's keys right
        for (int j = x.n; j > i; --j)
            x.k[j] = x.k[j - 1];
        // shift x's children right
        for (int j = x.n; j > i; --j)
            x.c[j + 1] = x.c[j];
        // insert key into x
        x.k[i] = y.k[t - 1];
        // insert z as child into x
        int zPos = allocatePos();
        x.c[i + 1] = zPos;
        // update x's size
        x.n += 1;
        // persistence
        setNode(x, pos);
        setNode(y, x.c[i]);
        setNode(z, x.c[i + 1]);

        return x;
    }

    Node _splitLeafChild(Node x, int pos, int i) {
        Leaf y = getLeaf(x.c[i]);
        Leaf z {};
        int zPos = allocatePos();
        // move last blockingFactor / 2 records to z
        int m = (blockingFactor + 1) / 2;
        for (int j = 0; j + m < blockingFactor; ++j)
            z.records[j] = y.records[j + m];
        // update y's size & next
        y.n = m;
        y.next = zPos;
        // update z's size
        z.n = blockingFactor - m;

        // shift x's keys right
        for (int j = x.n; j > i; --j)
            x.k[j] = x.k[j - 1];
        // shift x's children right
        for (int j = x.n; j > i; --j)
            x.c[j + 1] = x.c[j];
        // insert key into x
        x.k[i] = getKey(z.records[0]);
        // insert z as child into x
        x.c[i + 1] = zPos;
        // update x's size
        x.n += 1;
        // persistence
        setNode(x, pos);
        setLeaf(y, x.c[i]);
        setLeaf(z, x.c[i + 1]);

        return x;
    }

    void _splitRoot() {
        Node s {};
        s.leaf = false;
        s.c[0] = root;
        root = allocatePos();
        _splitChild(s, root, 0);
    }

    bool _insertNonFullLeaf(Leaf x, int pos, Key key, Record& record) {
        int match {};
        for (int k = 0; k < x.n; k >>= 1)
            while (match + k < x.n && getKey(x.records[match + k]) <= key) match += k;
        if (getKey(x.records[match]) == key) return false;

        x.records[x.n] = record;
        ++x.n;
        sort(x.records, x.records + x.n, recordCmp);

        setLeaf(x, pos);

        return true;
    }

    bool _insertNonFull(Node x, int pos, Key key, Record& record) {
        // find child to insert into
        int i {};
        for (; i < x.n; ++i)
            if (x.k[i] > key) break;

        if (x.leaf) {
            Leaf child = getLeaf(x.c[i]);
            if (child.n == blockingFactor) {
                x = _splitLeafChild(x, pos, i);
                if (x.k[i] <= key) ++i;
                child = getLeaf(x.c[i]);
            }
            return _insertNonFullLeaf(child, x.c[i], key);
        }
        else {
            Node child = getNode(x.c[i]);
            if (child.n == (t<<1)) {
                x = _splitChild(x, pos, i);
                if (x.k[i] <= key) ++i;
                child = getNode(x.c[i]);
            }
            return _insertNonFull(child, x.c[i], key);
        }
    }

    pair<Record,bool> _search(Node x, Key key) {

    }


public:
    BPlusTree(const char* filename, function<Key(Record&)> getKey, function<int(Key,Key)> cmp) : getKey(getKey), cmp(cmp) {
        strcpy(this->filename, filename);
        recordCmp = [&](Record& r1, Record& r2)->bool{ return cmp(getKey(r1), getKey(r2)) == -1; };

        fstream f (filename, ios::in | ios::out | ios::binary);
        bool exists = f.is_open();
        f.close();

        if (exists) getHeader();
        else {
            setHeader();
            Node r {};
            setNode(r, root);
        }
    }

    ~BPlusTree() = default;

    pair<Record,bool> search(Key key) {

    }

    bool insert(Record record) {
        Key key = getKey(record);

        // if root has no keys, create key and leaf + add record
        Node r = getNode(root);
        if (r.n == 0) {
            int lPos = allocatePos();
            int rPos = allocatePos();
            if (rPos == lPos) ++rPos;
            Leaf leftChild {};
            Leaf rightChild (1);
            rightChild.records[0] = record;

            r.k[0] = key;
            r.c[0] = lPos;
            r.c[1] = rPos;

            setNode(r, root);
            setLeaf(leftChild, lPos);
            setLeaf(rightChild, rPos);

            return true;
        }

        // split root if full
        if (r.n == (t<<1) - 1) {
            _splitRoot();
            r = getNode(root);
        }

        return _insertNonFull(r, root, key);
    }

    bool remove(Key key) {

    }

    vector<Record> range(Key left, Key right) {

    }
};

#endif //DB2_BPLUSTREE_BPLUSTREE_H
