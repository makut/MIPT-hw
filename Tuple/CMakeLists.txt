cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(Tuple)
set(CMAKE_CXX_FLAGS "-std=c++14 -O3 -W -Wextra -Wall -pedantic -Werror")

set(SOURCE_FILES Tuple.h test.cpp)
add_executable(Test ${SOURCE_FILES})
target_link_libraries(Test gtest)
target_link_libraries(Test pthread)
add_test(Test Test)
