cmake_minimum_required(VERSION 3.25)
project(pea_4)

set(CMAKE_CXX_STANDARD 17)

#Set Linker flags
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

file(COPY ${CMAKE_SOURCE_DIR}/source DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/config.ini DESTINATION ${CMAKE_BINARY_DIR})

add_executable(pea_4 main.cpp classes/TSP.cpp classes/TSP.h)
