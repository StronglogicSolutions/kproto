cmake_minimum_required(VERSION 3.10)

project(kproto)

set(CMAKE_CXX_STANDARD 20)

add_executable(kproto main.cpp)

target_link_libraries(kproto PRIVATE zmq)
