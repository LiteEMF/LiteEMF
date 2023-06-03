cmake_minimum_required(VERSION 3.15)

# 设置编译器路径
set(CMAKE_C_COMPILER "C:/MinGW/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW/bin/g++.exe")


project(LiteEMF_sample)

set(SUB_SRC "" CACHE STRING "init SUB_SRC" FORCE )
include_directories(./)
include_directories(./app)
include_directories(./api)
include_directories(./utils)
include_directories(./unit-test)
include_directories(./sample)
include_directories(./hal)

add_subdirectory(./app)
add_subdirectory(./api)
add_subdirectory(./utils)
add_subdirectory(./unit-test)
add_subdirectory(./sample)
add_subdirectory(./hal)

message("SUB_SRC=${SUB_SRC}")
add_executable(LiteEMF ${SUB_SRC})