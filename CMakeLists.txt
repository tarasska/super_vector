cmake_minimum_required(VERSION 2.8)

project(super_vector)

include_directories(${SUPER_VECTOR_SOURCE_DIR})

add_executable(vector_testing
               vector.hpp
               vector.cpp
               vector_testing.cpp
               counted.h
               counted.cpp
               fault_injection.h
               fault_injection.cpp
               gtest/gtest-all.cc
               gtest/gtest.h
               gtest/gtest_main.cc)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
endif()

target_link_libraries(vector_testing -lpthread)
