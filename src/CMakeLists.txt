cmake_minimum_required(VERSION 3.2)

project(planner)

add_definitions(-std=c++11)

set(HEADER_FILES
        PartialOrderPlan.h
        PartialOrderPlanner.h
        ComponentManager.h
        EntityManager.h
        schedules/Schedule.h
        schedules/ScheduleEntry.h
        schedules/Action.h
        schedules/ActionInstance.h
        schedules/ScheduleInstance.h
        components/ScheduleComponentManager.h
        components/LocationComponentManager.h
        components/PositionComponentManager.h
        ActionManager.h
        framework/Game.h
        framework/Window.h
        framework/Graphics.h
        framework/Input.h
        framework/Keyboard.h
        util/Extra.h
        schedules/Predicate.h
        WorldState.h)

set(SOURCE_FILES
        PartialOrderPlanner.cpp
        EntityManager.cpp
        schedules/Schedule.cpp
        schedules/ScheduleEntry.cpp
        schedules/Action.cpp
        schedules/ActionInstance.cpp
        schedules/ScheduleInstance.cpp
        components/ScheduleComponentManager.cpp
        components/LocationComponentManager.cpp
        components/PositionComponentManager.cpp
        ActionManager.cpp
        framework/Window.cpp
        framework/Graphics.cpp
        framework/Input.cpp
        framework/Keyboard.cpp
        components/CharacterComponentManager.h)

add_library(planner STATIC ${SOURCE_FILES} ${HEADER_FILES})