cmake_minimum_required(VERSION 3.20)
project(sql_parser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -lstdc++ -Wall -Werror")

add_executable(sql_parser main.cpp)

include_directories(./src)
link_directories(./)
target_link_libraries(sql_parser ${CMAKE_SOURCE_DIR}/libsqlparser.so)
