cmake_minimum_required(VERSION 3.22)

project(interpreter CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(main main.cpp matrix.cpp interpreter.cpp)
