# CMake config
# knx application

# min version
cmake_minimum_required(VERSION 2.8)

# project name
project( knxApp )

# sub
add_subDirectory (build/android)
add_subDirectory (build/ubuntu)
