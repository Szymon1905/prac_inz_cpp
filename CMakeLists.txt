
cmake_minimum_required(VERSION 3.24)

project(Prac_inz_c++)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(PracInzCpp main.cpp
        genetyczny.cpp
        genetyczny.h
        mutacje.cpp mutacje.h osobnik.cpp osobnik.h)
