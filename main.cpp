#include <iostream>
#include "BPlusTree.h"
#include "Student.h"
using namespace std;

const char* filename = "bplustree.bin";
auto cmp = [](const char* k1, const char* k2)->int{
    int c = strcmp(k1, k2);
    if (c < 0) return -1;
    if (c > 0) return 1;
    return c;
};
auto getKey = [](Student& s){ return s.getCode(); };

int main() {
    BPlusTree<Student,const char*,5*sizeof(char)> btree (filename, getKey, cmp);
    return 0;
}
