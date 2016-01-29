cmake_minimum_required(VERSION 3.2)

project(planner)

add_definitions(-std=c++11)

set(HEADER_FILES
        PartialOrderPlan.h
        PartialOrderPlanner.h
        ComponentManager.h
        ComponentManagers.h
        EntityManager.h
        tests/Sussman.h
        tests/FruitBowl.h
        tests/Table.h
        tests/WorldLocation.h
        tests/Krulg.h
        extras.h
        )

set(SOURCE_FILES
        PartialOrderPlanner.cpp
        EntityManager.cpp
        )

add_library(planner STATIC ${SOURCE_FILES} ${HEADER_FILES})