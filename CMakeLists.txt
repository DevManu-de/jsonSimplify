cmake_minimum_required(VERSION 3.14)

project(jsonSimplify LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(jsonSimplify STATIC
  src/json_simplify.cpp
  include/json_simplify.hpp
)

target_compile_definitions(jsonSimplify PRIVATE JSONSIMPLIFY_LIBRARY)
