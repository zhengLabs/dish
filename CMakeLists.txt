cmake_minimum_required(VERSION 3.20)
project(dish)

set(CMAKE_CXX_STANDARD 17)

include_directories(./include/)
include_directories(/usr/local/include/)


add_executable(analysisTest
        analysisTest.cpp
        analysis.cpp
        hgraph.cpp 
        util.cpp
        )
