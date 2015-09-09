cmake_minimum_required(VERSION 2.8)
project(PIET_I)
add_definitions("-std=c++14 -g -Og -march=native -mtune=native -Wall -Wextra")
add_executable(piet-i
  src/main.cpp
  src/visualize.cpp
)
target_link_libraries(piet-i png16)
