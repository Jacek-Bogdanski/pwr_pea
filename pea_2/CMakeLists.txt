cmake_minimum_required(VERSION 3.25)
project(pea_1)

set(CMAKE_CXX_STANDARD 17)

#Set Linker flags
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

file(COPY ${CMAKE_SOURCE_DIR}/source DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

add_executable(pea_1 main.cpp)
