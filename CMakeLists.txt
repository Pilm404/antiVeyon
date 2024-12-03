cmake_minimum_required(VERSION 3.29)
project(antiVeyon)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
set(CMAKE_CXX_STANDARD 20)

add_executable(antiVeyon main.cpp)
