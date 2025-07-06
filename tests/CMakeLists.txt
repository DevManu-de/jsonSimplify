cmake_minimum_required(VERSION 3.14)

project(jsonSimplify LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include_directories(../include)

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cases DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

foreach(testSrc ${TEST_SRCS})
    get_filename_component(testName ${testSrc} NAME_WE)
    add_executable(${testName} ${testSrc})

    target_link_libraries(${testName} jsonSimplify)

    add_test(NAME ${testName}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${testName}
            )
endforeach(testSrc)
