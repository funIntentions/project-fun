cmake_minimum_required(VERSION 3.2)

project(planner)

add_definitions(-std=c++11)

set(HEADER_FILES
        PartialOrderPlan.h
        PartialOrderPlanner.h
        ComponentManager.h
        EntityManager.h
        util/Extra.h
        schedules/Schedule.h
        schedules/ActionInstance.cpp
        schedules/ActionInstance.h
        schedules/ScheduleInstance.cpp
        schedules/ScheduleInstance.h
        ActionManager.h
        WorldState.h)

set(SOURCE_FILES
        PartialOrderPlanner.cpp
        EntityManager.cpp
        schedules/Schedule.cpp
        schedules/ScheduleEntry.cpp
        schedules/ScheduleEntry.h
        schedules/Action.cpp
        schedules/Action.h
        schedules/ActionInstance.cpp
        schedules/ActionInstance.h
        schedules/ScheduleInstance.cpp
        schedules/ScheduleInstance.h
        ScheduleComponentManager.cpp
        ScheduleComponentManager.h
        framework/Game.h
        framework/Window.cpp
        framework/Window.h
        framework/Graphics.cpp
        framework/Graphics.h
        framework/Input.cpp
        framework/Input.h
        framework/Keyboard.cpp
        framework/Keyboard.h
        schedules/Predicate.h
        ActionManager.cpp)

add_library(planner STATIC ${SOURCE_FILES} ${HEADER_FILES})