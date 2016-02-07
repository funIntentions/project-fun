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
        Schedules/Schedule.h)

set(SOURCE_FILES
        PartialOrderPlanner.cpp
        EntityManager.cpp
        Schedules/Schedule.cpp Schedules/ScheduleEntry.cpp Schedules/ScheduleEntry.h Schedules/Action.cpp Schedules/Action.h)

add_library(planner STATIC ${SOURCE_FILES} ${HEADER_FILES})