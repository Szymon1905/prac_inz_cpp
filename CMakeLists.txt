
cmake_minimum_required(VERSION 3.24)

project(PEA_genetyczny)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(PEA_genetyczny main.cpp
        genetyczny.cpp
        genetyczny.h
        mutacje.cpp mutacje.h osobnik.cpp osobnik.h)
