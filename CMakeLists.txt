cmake_minimum_required(VERSION 3.14)

project(jsonSimplify LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(jsonSimplify STATIC
  src/json.cpp
  src/json_array.cpp
  src/json_element.cpp
  src/json_object.cpp
  src/json_simplify.cpp
  src/json_value.cpp

  include/json_simplify.hpp
  include/json.hpp
  include/json_element.hpp
  include/json_value.hpp
  include/json_object.hpp
  include/json_array.hpp

)

target_compile_definitions(jsonSimplify PRIVATE JSONSIMPLIFY_LIBRARY)

if(("$ENV{JSON_SIMPLIFY_TESTS}" STREQUAL "true") OR
   ("$ENV{JSON_SIMPLIFY_TESTS}" STREQUAL "TRUE") OR
   ("$ENV{JSON_SIMPLIFY_TESTS}" STREQUAL "1"))
    add_subdirectory(tests)
endif()
