cmake_minimum_required(VERSION 3.2)

project(story_engine)

add_definitions(-std=c++11)

set(HEADER_FILES
        planner/PartialOrderPlan.h
        planner/PartialOrderPlanner.h
        framework/ComponentManager.h
        framework/EntityManager.h
        schedules/ScheduleData.h
        schedules/ScheduleEntry.h
        schedules/ActionData.h
        schedules/Action.h
        schedules/Schedule.h
        components/ScheduleComponentManager.h
        components/LocationComponentManager.h
        components/PositionComponentManager.h
        schedules/ActionManager.h
        framework/Game.h
        framework/Window.h
        framework/Graphics.h
        framework/Input.h
        framework/Keyboard.h
        util/Extra.h
        schedules/Predicate.h)

set(SOURCE_FILES
        planner/PartialOrderPlanner.cpp
        framework/EntityManager.cpp
        schedules/ScheduleData.cpp
        schedules/ScheduleEntry.cpp
        schedules/ActionData.cpp
        schedules/Action.cpp
        schedules/Schedule.cpp
        components/ScheduleComponentManager.cpp
        components/LocationComponentManager.cpp
        components/PositionComponentManager.cpp
        schedules/ActionManager.cpp
        framework/Window.cpp
        framework/Graphics.cpp
        framework/Input.cpp
        framework/Keyboard.cpp
        components/OpinionComponentManager.h
        framework/Shader.cpp
        framework/Shader.h
        framework/TextRenderer.cpp
        framework/TextRenderer.h
        components/AttributeComponentManager.cpp
        components/AttributeComponentManager.h
        demo/StoryEngineGame.cpp
        demo/StoryEngineGame.h
        demo/PlannerGame.cpp
        demo/PlannerGame.h
        components/DescriptionComponentManager.cpp
        components/DescriptionComponentManager.h
        components/PlaceComponentManager.cpp
        components/PlaceComponentManager.h
        domain/Adventure.h
        components/Attribute.h
        framework/Cursor.h
        components/OwnershipComponentManager.cpp
        components/OwnershipComponentManager.h
        components/TypeComponentManager.h
        demo/Constants.h
        demo/StoryLogger.h
        components/StateComponentManager.h
        components/StateComponentManager.cpp)

add_library(story_engine STATIC ${SOURCE_FILES} ${HEADER_FILES})