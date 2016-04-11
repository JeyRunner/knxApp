# CMake config
# knx server and client project

# min version
cmake_minimum_required(VERSION 2.8)

# project name
project(knx)

include_directories(protocol/src/  protocol/src/log/)

# source files
set (SRC_DIR_PROTOCOL			protocol/src/)
file(GLOB SRC_FILES_PROTOCOL   ${SRC_DIR_PROTOCOL}*.cpp ${SRC_DIR_PROTOCOL}*.h   ${SRC_DIR_PROTOCOL}log/*.cpp ${SRC_DIR_PROTOCOL}log/*.h)

# sub
add_subDirectory (protocol/)
add_subDirectory (server/)
add_subDirectory (client/)
