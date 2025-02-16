cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(kproto main.cpp)

# Link against ZeroMQ
target_link_libraries(kproto PRIVATE zmq)

