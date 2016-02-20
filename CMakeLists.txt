cmake_minimum_required(VERSION 3.2)
project(PartialOrderPlanner)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")


set(SOURCE_FILES main.cpp)

include_directories(
        src
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.1.2.win32/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-1.13.0/include"
)

link_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.1.2.win32/lib-mingw"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-1.13.0/lib/Release/Win32"
)

add_subdirectory(src)
add_subdirectory(test)

add_executable(run_planner ${SOURCE_FILES} )

target_link_libraries(run_planner ${OPENGL_LIBRARIES} planner glfw3 glew32s glu32 opengl32)