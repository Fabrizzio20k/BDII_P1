cmake_minimum_required(VERSION 3.20)
project(BDII)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows")
elseif(APPLE)
    set(CMAKE_PREFIX_PATH "/usr/local/Cellar")
else()
    set(CMAKE_PREFIX_PATH "/usr/local")
endif()

find_package(Crow CONFIG REQUIRED)

include_directories(include dataStructures/avlFile)
include_directories(include dataStructures/dbs)

add_executable(main src/main.cpp
        include/api.h)

if(WIN32)
    target_link_libraries(main PRIVATE Crow::Crow ws2_32 wsock32)
else()
    target_link_libraries(main PRIVATE Crow::Crow)
endif()


add_executable(Hash dataStructures/extendibleHashing/extendibleHash.cpp)