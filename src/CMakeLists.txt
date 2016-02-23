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
        schedules/Schedule.h schedules/ActionInstance.cpp schedules/ActionInstance.h schedules/ScheduleInstance.cpp schedules/ScheduleInstance.h)

set(SOURCE_FILES
        PartialOrderPlanner.cpp
        EntityManager.cpp
        schedules/Schedule.cpp schedules/ScheduleEntry.cpp schedules/ScheduleEntry.h schedules/Action.cpp schedules/Action.h schedules/ActionInstance.cpp schedules/ActionInstance.h schedules/ScheduleInstance.cpp schedules/ScheduleInstance.h ScheduleComponentManager.cpp ScheduleComponentManager.h framework/Game.h framework/Window.cpp framework/Window.h framework/Graphics.cpp framework/Graphics.h framework/Input.cpp framework/Input.h framework/Keyboard.cpp framework/Keyboard.h schedules/Predicate.h)

add_library(planner STATIC ${SOURCE_FILES} ${HEADER_FILES})