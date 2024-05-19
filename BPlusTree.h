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
    int lastDel {-1}; // pointer to last deleted block
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
        if (lastDel >= 0) {
            Leaf leaf = getLeaf(lastDel);
            pos = lastDel;
            lastDel = leaf.nextDel;

            setHeader();
            return pos;
        }
        else {
            fstream f (filename, ios::in | ios::out | ios::binary);
            f.seekg(0,ios::end);
            int fuckyou = f.tellg();
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
        f.write(reinterpret_cast<char*>(&leaf), blockSize);
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

    Node _splitChild(Node& x, int pos, int i) {
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

    Node _splitLeafChild(Node& x, int pos, int i) {
        Leaf y = getLeaf(x.c[i]);
        Leaf z {};
        int zPos = allocatePos();
        // move last blockingFactor / 2 records to z
        int m = (blockingFactor + 1) / 2;
        for (int j = 0; j + m < blockingFactor; ++j)
            z.records[j] = y.records[j + m];
        // update z's size
        z.n = blockingFactor - m;
        z.next = y.next;
        // update y's size & next
        y.n = m;
        y.next = zPos;

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
        setNode(s, root);
        setHeader();
        _splitChild(s, root, 0);
    }

    bool _insertNonFullLeaf(Leaf& x, int pos, Key key, Record& record) {
        int match {};
        for (int k = x.n - 1; k > 0; k >>= 1)
            while (match + k < x.n && getKey(x.records[match + k]) <= key) match += k;
        if (getKey(x.records[match]) == key) return false;

        x.records[x.n] = record;
        ++x.n;
        sort(x.records, x.records + x.n, recordCmp);

        setLeaf(x, pos);

        return true;
    }

    bool _insertNonFull(Node& x, int pos, Key key, Record& record) {
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
            return _insertNonFullLeaf(child, x.c[i], key, record);
        }
        else {
            Node child = getNode(x.c[i]);
            if (child.n == (t<<1) - 1) {
                x = _splitChild(x, pos, i);
                if (x.k[i] <= key) ++i;
                child = getNode(x.c[i]);
            }
            return _insertNonFull(child, x.c[i], key, record);
        }
    }

    pair<Key,bool> _removeFromLeaf(Leaf& x, int pos, Key key) { // returns {predecessor/successor, wasRemoved}
        int match {};
        for (int k = x.n - 1; k > 0; k >>= 1)
            while (match + k < x.n && getKey(x.records[match + k]) <= key) match += k;
        if (getKey(x.records[match]) != key) return {Key{},false};

        Key replacement {};
        if (match == 0) replacement = getKey(x.records[1]);
        else replacement = getKey(x.records[0]);

        for (int i = match + 1; i < x.n; ++i)
            x.records[i - 1] = x.records[i];
        x.n -= 1;

        setLeaf(x, pos);

        return {replacement, true};
    }

    void getKeyFromLeft(Node& x, int pos, int i) {
        if (x.leaf) {
            Leaf child = getLeaf(x.c[i]);
            Leaf leftChild = getLeaf(x.c[i - 1]);
            for (int j = child.n; j > 0; --j)
                child.records[j] = child.records[j - 1];

            child.records[0] = leftChild.records[leftChild.n - 1];
            child.n += 1;
            leftChild.n -= 1;

            x.k[i - 1] = getKey(child.records[0]);

            setNode(x, pos);
            setLeaf(leftChild, x.c[i - 1]);
            setLeaf(child, x.c[i]);
        }
        else {
            Node child = getNode(x.c[i]);
            Node leftChild = getNode(x.c[i - 1]);

            for (int j = child.n; j > 0; --j)
                child.k[j] = child.k[j - 1];
            for (int j = child.n + 1; j > 0; --j)
                child.c[j] = child.c[j - 1];

            child.k[0] = x.k[i];
            child.c[0] = leftChild.c[leftChild.n];
            x.k[i - 1] = leftChild.k[leftChild.n - 1];

            child.n += 1;
            leftChild.n -= 1;

            setNode(x, pos);
            setNode(child, x.c[i]);
            setNode(leftChild, x.c[i - 1]);
        }
    }

    void getKeyFromRight(Node& x, int pos, int i) {
        if (x.leaf) {
            Leaf child = getLeaf(x.c[i]);
            Leaf rightChild = getLeaf(x.c[i + 1]);
            child.records[child.n] = rightChild.records[0];
            for (int j = 1; j < rightChild.n; ++j)
                rightChild.records[j - 1] = rightChild.records[j];

            child.n += 1;
            rightChild.n -= 1;

            x.k[i] = rightChild.records[0];

            setNode(x, pos);
            setLeaf(rightChild, x.c[i + 1]);
            setLeaf(child, x.c[i]);
        }
        else {
            Node child = getNode(x.c[i]);
            Node rightChild = getNode(x.c[i + 1]);

            child.k[child.n] = x.k[i];
            x.k[i] = rightChild.k[0];
            for (int j = 1; j < rightChild.n; ++j)
                rightChild.k[j - 1] = rightChild.k[j];

            child.c[child.n + 1] = rightChild.c[0];
            for (int j = 1; j <= rightChild.n; ++j)
                rightChild.c[j - 1] = rightChild.c[j];

            child.n += 1;
            rightChild.n -= 1;

            setNode(x, pos);
            setNode(child, x.c[i]);
            setNode(rightChild, x.c[i + 1]);
        }
    }

    void mergeWithLeft(Node& x, int pos, int i) {
        if (x.leaf) {
           Leaf child = getLeaf(x.c[i]);
           Leaf leftChild = getLeaf(x.c[i - 1]);

           for (int j = 0; j < child.n; ++j)
               leftChild.records[j + leftChild.n] = child.records[j];

            Leaf del {};
            del.nextDel = lastDel;
            lastDel = x.c[i];

           for (int j = i; j < x.n; ++j)
               x.k[i - 1] = x.k[i];

           for (int j = i + 1; j <= x.n; ++j)
               x.c[j - 1] = x.c[j];

           x.n -= 1;
           leftChild.n += child.n;

            setNode(x, pos);
            setLeaf(leftChild, x.c[i - 1]);
            setLeaf(del, lastDel);
            setHeader();
        }
        else {
            Node child = getNode(x.c[i]);
            Node leftChild = getNode(x.c[i - 1]);

            leftChild.k[leftChild.n] = x.k[i];
            for (int j = 1; j <= child.n; ++j)
                leftChild.k[j + leftChild.n] = child[j - 1];

            for (int j = 1; j <= child.n + 1; ++j)
                leftChild.c[j + leftChild.n] = child.c[j - 1];

            Leaf del {};
            del.nextDel = lastDel;
            lastDel = x.c[i];

            for (int j = i; j < x.n; ++j)
                x.k[i - 1] = x.k[i];

            for (int j = i + 1; j <= x.n; ++j)
                x.c[j - 1] = x.c[j];

            x.n -= 1;
            leftChild.n += child.n;

            setNode(x, pos);
            setNode(leftChild, x.c[i - 1]);
            setLeaf(del, lastDel);
            setHeader();
        }
    }

    pair<Key,bool> _remove(Node& x, int pos, Key key) {
        // find child to recurse into
        int i {}, m {};
        for (; i < x.n; ++i)
            if (x.k[i] > key) break;

        Key replacement {};
        bool wasRemoved {};
        if (x.leaf) {
            m = (blockingFactor>>1);
            Leaf child = getLeaf(x.c[i]);
            if (child.n == m) {
                Leaf leftChild {};
                Leaf rightChild {};
                if (i > 0) leftChild = getLeaf(x.c[i - 1]);
                if (i < x.n) rightChild = getLeaf(x.c[i + 1]);

                if (leftChild.n > m) getKeyFromLeft(x, pos, i);
                else if (rightChild.n > m) getKeyFromRight(x, pos, i);
                else if (i > 0) {
                    mergeWithLeft(x, pos, i);
                    i -= 1;
                }
                else mergeWithLeft(x, pos, i + 1);
            }
            child = getLeaf(x.c[i]);

            auto p = _removeFromLeaf(child, x.c[i], key);
            replacement = p.fi;
            wasRemoved = p.se;
        }
        else {
            m = t - 1;
            Node child = getNode(x.c[i]);
            if (child.n == m) {
                Node leftChild {};
                Node rightChild {};
                if (i > 0) leftChild = getNode(x.c[i - 1]);
                if (i < x.n) rightChild = getNode(x.c[i + 1]);

                if (leftChild.n > m) getKeyFromLeft(x, pos, i);
                else if (rightChild.n > m) getKeyFromRight(x, pos, i);
                else if (i > 0) {
                    mergeWithLeft(x, pos, i);
                    i -= 1;
                }
                else mergeWithLeft(x, pos, i + 1);
            }
            child = getNode(x.c[i]);

            auto p = _remove(child, x.c[i], key);
            replacement = p.fi;
            wasRemoved = p.se;
        }

        if (wasRemoved && i > 0 && x.k[i - 1] == key) {
            x.k[i - 1] = replacement;
            setNode(x, pos);
        }
        return {replacement, wasRemoved};
    }

    pair<Record,bool> _search(Leaf x, Key key) {
        int i {};
        for (int k = x.n - 1; k > 0; k >>= 1)
            while (i + k < x.n && getKey(x.records[i + k]) <= key) i += k;
        if (getKey(x.records[i]) == key) return {x.records[i], true};

        return {Record{}, false};
    }

    pair<Record,bool> _search(Node x, Key key) {
        int i {};
        for (; i < x.n; ++i)
            if (x.k[i] > key) break;
        if (x.leaf) {
            Leaf y = getLeaf(x.c[i]);
            return _search(y, key);
        }
        else {
            Node y = getNode(x.c[i]);
            return _search(y, key);
        }
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
            createHeader();
            Node r {};
            setNode(r, root);
        }
    }

    ~BPlusTree() = default;

    pair<Record,bool> search(Key key) {
        Node r = getNode(root);
        if (r.n == 0) return {Record{}, false};

        return _search(r, key);
    }

    bool insert(Record record) {
        Key key = getKey(record);

        // if root has no keys, create key and leaf + add record
        Node r = getNode(root);
        if (r.c[0] == 0) {
            int lPos = allocatePos();
            int rPos = allocatePos();
            if (rPos == lPos) ++rPos;
            Leaf leftChild {};
            Leaf rightChild (1);
            leftChild.next = rPos;
            rightChild.records[0] = record;

            ++r.n;
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

        return _insertNonFull(r, root, key, record);
    }

    bool remove(Key key) {
        Node r = getNode(root);
        auto [replacement, wasRemoved] = _remove(r, root, key);
        return wasRemoved;
    }

    vector<Record> range(Key left, Key right) {

    }
};

#endif //DB2_BPLUSTREE_BPLUSTREE_H
