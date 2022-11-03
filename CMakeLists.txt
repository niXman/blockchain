cmake_minimum_required(VERSION 2.8)
project(blockchain)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
endif()

add_executable(
    ${PROJECT_NAME}
    main.cpp
    helpers.hpp
    blockchain.hpp
    storage.hpp
)
