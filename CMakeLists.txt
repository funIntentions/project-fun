cmake_minimum_required(VERSION 3.2)
project(PartialOrderPlanner)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp tests/Krulg.h)
add_executable(PartialOrderPlanner ${SOURCE_FILES} PartialOrderPlan.h PartialOrderPlanner.cpp PartialOrderPlanner.h tests/Sussman.h tests/Briefcase.h tests/FruitBowl.h tests/Table.h helper/extras.h component/ComponentManager.h entity/EntityManager.h entity/EntityManager.cpp component/ComponentManagers.h tests/WorldLocation.h tests/WorldLocation.h)