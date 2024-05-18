#include <iostream>
#include "BPlusTree.h"
#include "Student.h"
#include <cassert>
using namespace std;

const char* filename = "bplustree.bin";

auto cmp = [](int k1, int k2)->int{
    if (k1 < k2) return -1;
    if (k1 > k2) return 1;
    return 0;
};

auto getKey = [](Student& s){ return s.getId(); };

void testInsert(vector<Student>& students) {
    BPlusTree<Student,int,sizeof(int)> btree (filename, getKey, cmp);

    for (auto student: students) {
        btree.insert(student);
    }

    size_t i = size(students);
    students.emplace_back(20, "Sachi", "Yui", "INF", 5, 2000.80);
    students.emplace_back(19, "Lyzbeth", "Shinozaki", "PSI", 4, 1020.80);

    size_t n = size(students);
    for (; i < n; ++i) {
        btree.insert(students[i]);
//        if (!btree.search(students[i].getId()).second) {
//            cout << students[i].getId() << '\n';
//            break;
//        }
    }
}

void testSearch(vector<Student>& students) {
    BPlusTree<Student,int,sizeof(int)> btree (filename, getKey, cmp);

    for (auto student: students) {
        cout << student.getId() << '\n';
        assert(btree.search(student.getId()).second);
    }
    assert(!btree.search(0).second);
    assert(!btree.search(15).second);
    assert(!btree.search(18).second);
}

int main() {
    vector<Student> students = {
            {8, "Tito", "Luna", "IND", 4, 1550.20},
            {9, "Lola", "Nuez", "INF", 1, 1150.30},
            {10, "Paco", "Rio", "ADM", 2, 1050.90},
            {3, "Mia", "Lopez", "DER", 3, 1300.75},
            {14, "Juan", "Diaz", "MED", 4, 1400.25},
            {5, "Sara", "Mora", "PSI", 1, 1250.00},
            {17, "Rita", "Solis", "CIV", 3, 1450.60},
            {1, "Ana", "Perez", "INF", 1, 1200.50},
            {2, "Luis", "Gomez", "ADM", 2, 1100.00},
            {6, "Leo", "Vega", "ARQ", 2, 1350.45}
    };

    for (int i = 0; i < 200; ++i) {
        string surname = "surname" + to_string(i);
        string lastname = "lastname" + to_string(i);
        students.emplace_back(i + 21, surname.c_str(), lastname.c_str(), "CSC", 9, 3500);
    }

    testInsert(students);
    testSearch(students);

    return 0;
}
