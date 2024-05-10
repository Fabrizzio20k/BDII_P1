cmake_minimum_required(VERSION 3.20)
project(BDII)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(Crow_DIR "C:/vcpkg/packages/crow_x64-windows/share/crow")
    set(ASIO_INCLUDE_DIR "C:/vcpkg/packages/asio_x64-windows/include")
elseif(APPLE)
    set(Crow_DIR "/usr/local/share/crow")
    set(ASIO_INCLUDE_DIR "/usr/local/include")
else()
    set(Crow_DIR "/usr/share/crow")
    set(ASIO_INCLUDE_DIR "/usr/include")
endif()

find_package(Crow CONFIG REQUIRED)

include_directories(include)

add_executable(main src/main.cpp)

if(WIN32)
    target_link_libraries(main PRIVATE Crow::Crow ws2_32 wsock32)
else()
    target_link_libraries(main PRIVATE Crow::Crow)
endif()
