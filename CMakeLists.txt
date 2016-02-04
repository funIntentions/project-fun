cmake_minimum_required(VERSION 3.2)
project(PartialOrderPlanner)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")


set(SOURCE_FILES main.cpp)
add_executable(run_planner ${SOURCE_FILES} )

include_directories(src include)

add_subdirectory(src)
add_subdirectory(test)

target_link_libraries(run_planner planner)