cmake_minimum_required(VERSION 3.27)
project(db2_bplustree)

set(CMAKE_CXX_STANDARD 17)

add_executable(db2_bplustree main.cpp
        BPlusTree.h
        Student.h)
