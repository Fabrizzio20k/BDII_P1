//
// Created by iansg on 5/18/2024.
//

#ifndef DB2_BPLUSTREE_STUDENT_H
#define DB2_BPLUSTREE_STUDENT_H

#include <iostream>
#include <cstring>
using namespace std;

class Student {
    char code[5]{};
    char surname[11]{};
    char lastName[20]{};
    char career[15]{};
    int semester {};
    float monthlyPayment {};
public:
    Student() = default;
    Student(const char* code, const char* surname, const char* lastName, const char* career, int semester, float monthlyPayment) : semester(semester), monthlyPayment(monthlyPayment) {
        strcpy(this->code, code);
        strcpy(this->surname, surname);
        strcpy(this->lastName, lastName);
        strcpy(this->career, career);
    }
    const char* getCode() { return code; }
    friend bool operator==(const Student& s1, const Student& s2) {
        return strcmp(s1.code, s2.code) == 0 && strcmp(s1.surname, s2.surname) == 0
               && strcmp(s1.lastName, s2.lastName) == 0 && strcmp(s1.career, s2.career) == 0
               && s1.semester == s2.semester && s1.monthlyPayment == s2.monthlyPayment;
    }
    friend bool operator!=(const Student& s1, const Student& s2) {
        return !(s1==s2);
    }
    friend ostream& operator<<(ostream& out, const Student& s) {
        out << s.code << " | " << s.surname << " | " << s.lastName << " | " <<  s.career << "-" << s.semester << " | " << s.monthlyPayment << '\n';
        return out;
    }
};

#endif //DB2_BPLUSTREE_STUDENT_H
