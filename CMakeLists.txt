cmake_minimum_required(VERSION 3.20)
project(dish)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(/usr/local/include/)


add_executable(analysisTest
        analysisTest.cpp
        )
add_executable(program
        test_kahypar.cpp
        )