cmake_minimum_required(VERSION 3.2)
project(StoryEngine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -static-libgcc -static-libstdc++  -D__NO_INLINE__ ")


set(SOURCE_FILES main.cpp)

include_directories(
        src
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.1.2.win32/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-1.13.0/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm-0.9.7.1"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeType/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeType/include/freetype2"
)

link_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.1.2.win32/lib-mingw"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew-1.13.0/lib/Release/Win32"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeType/lib"
)

add_subdirectory(src)
add_subdirectory(test)

add_executable(run_demo ${SOURCE_FILES} )

target_link_libraries(run_demo story_engine ${OPENGL_LIBRARIES} freetype glfw3 glew32s glu32 opengl32)