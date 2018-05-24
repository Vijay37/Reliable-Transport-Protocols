cmake_minimum_required(VERSION 3.8)
project(cse489589_assignment2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        grader/advanced_tests
        grader/basic_tests
        grader/run_experiments
        grader/sanity_tests
        vijayaha/include/simulator.h
        vijayaha/src/abt.cpp
        vijayaha/src/gbn.cpp
        vijayaha/src/simulator.cpp
        vijayaha/src/sr.cpp)

add_executable(cse489589_assignment2 ${SOURCE_FILES})