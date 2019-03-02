cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(XOR-List)
set(CMAKE_CXX_FLAGS "-std=c++14 -O3")

set(SOURCE_FILES XorList.h StackAllocator.h test1.cpp)
add_executable(Test ${SOURCE_FILES})
target_link_libraries(Test gtest)
target_link_libraries(Test pthread)
add_test(Test Test)