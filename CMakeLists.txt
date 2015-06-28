cmake_minimum_required(VERSION 3.2)
project(PartialOrderPlanner)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(PartialOrderPlanner ${SOURCE_FILES} PartialOrderPlan.cpp PartialOrderPlan.h PartialOrderPlanner.cpp PartialOrderPlanner.h tests/Sussman.h)