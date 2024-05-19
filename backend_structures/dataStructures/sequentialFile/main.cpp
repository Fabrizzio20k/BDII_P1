#include "SequentialFile.h"
#include <cstring>
#include "Student.h"
#include <cassert>

const char* filename = "sf.bin";

auto getKey = [](Student& s)->const char*{ return s.getCode(); };
auto cmp = [](const char* s1, const char* s2)->int{
    int c = strcmp(s1, s2);
    if (c < 0) return -1;
    if (c > 0) return 1;
    return c;
};

void testInsert(vector<Student>& students) {
    SequentialFile<Student, const char *> sf(filename, getKey, cmp);

    for (auto student: students) {
        sf.insert(student);
        sf.print();
    }
}

void testSearch(vector<Student>& students) {
    SequentialFile<Student,const char*> sf (filename, getKey, cmp);
    size_t n = size(students);

    for (size_t i = 0; i < n; ++i) {
        //cout << i << endl;
        assert(sf.search(students[i].getCode()).second);
    }
}

void testRangeSearch(vector<Student>& students) {
    SequentialFile<Student,const char*> sf (filename, getKey, cmp);
    vector<Student> result = sf.rangeSearch("A000", "A006");
    for (auto student : result)
        cout << student.getCode() << '\n';
}

void testDelete(vector<Student> students) {
    SequentialFile<Student,const char*> sf (filename, getKey, cmp);

    size_t n = size(students);

    for (size_t i = 0; i*2 < n; ++i) {
        cout << i << endl;
        assert(sf.remove(students[i].getCode()));
    }

    for (size_t i = 0; i*2 < n; ++i)
        assert(!sf.remove(students[i].getCode()));

    for (size_t i = n/2; i < n; ++i)
        assert(sf.search(students[i].getCode()).second);
}

int main() {
    vector<Student> students = {
            {"A010", "Paco", "Rio", "ADM", 2, 1050.90},
            {"A002", "Luis", "Gomez", "ADM", 2, 1100.00},
            {"A001", "Ana", "Perez", "INF", 1, 1200.50},
            {"A005", "Sara", "Mora", "PSI", 1, 1250.00},
            {"A009", "Lola", "Nuez", "INF", 1, 1150.30},
            {"A006", "Leo", "Vega", "ARQ", 2, 1350.45},
            {"A003", "Mia", "Lopez", "DER", 3, 1300.75},
            {"A007", "Rita", "Solis", "CIV", 3, 1450.60},
            {"A008", "Tito", "Luna", "IND", 4, 1550.20},
            {"A004", "Juan", "Diaz", "MED", 4, 1400.25},
            {"A000", "Lyzbeth", "Shinozaki", "PSI", 4, 1020.80},
            {"A012", "Sachi", "Yui", "INF", 5, 2000.80}
    };

    testInsert(students); // comment for second run to test persistence
    testSearch(students);
    testRangeSearch(students);
    testDelete(students);

    return 0;
}
