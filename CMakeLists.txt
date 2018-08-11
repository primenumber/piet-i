cmake_minimum_required(VERSION 2.8)
project(PIET_I)
set(CMAKE_CXX_FLAGS "-std=c++17 -pg -fopenmp -g -Og -march=native -mtune=native -Wall -Wextra")
set(CMAKE_LD_FLAGS "-pg -fopenmp")
add_executable(piet-i
  src/main.cpp
  src/interpret.cpp
  src/pas.cpp
  src/io32.cpp
  src/utils.cpp
  src/codel.cpp
  src/parser.cpp
  src/color_blocks.cpp
  src/fillmap.cpp
)
target_link_libraries(piet-i png16)
