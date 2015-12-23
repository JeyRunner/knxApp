# CMake config
# knx server and client project

# min version
cmake_minimum_required(VERSION 2.8)

# project name
project(knx)

include_directories(protocol/src/)

# sub
add_subDirectory (protocol/)
add_subDirectory (server/)
add_subDirectory (client/)
