cmake_minimum_required(VERSION 3.2)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

SET(HEADER_FILES
        domains/Briefcase.h
        domains/FruitBowl.h
        domains/Sussman.h
        domains/Table.h)

SET(SOURCE_FILES
        PartialOrderPlannerFixture.cpp OpinionComponentFixture.cpp AttributeComponentFixture.cpp ScheduleComponentFixture.cpp)

add_executable(run_tests ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(run_tests gtest gtest_main)
target_link_libraries(run_tests planner)