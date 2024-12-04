cmake_minimum_required(VERSION 3.29)
project(veyonServiceDetector)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
set(CMAKE_CXX_STANDARD 20)

add_executable(veyonServiceDetector main.cpp)

target_link_libraries(veyonServiceDetector PRIVATE Iphlpapi ws2_32)
