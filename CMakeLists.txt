cmake_minimum_required(VERSION 2.8)
project(blockchain)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

add_executable(blockchain main.cpp)
