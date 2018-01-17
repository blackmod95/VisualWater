cmake_minimum_required(VERSION 3.7)
project(VisualWater)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)

add_subdirectory(glfw)
add_subdirectory(glxw)

include_directories(glfw/include)
include_directories(glxw/include)
include_directories(glm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -std=c++11")

set(SOURCE_FILES
        main.cpp
        utils/camera.h
        utils/camera.cpp
        utils/utils.h
        utils/utils.cpp
        )
add_executable(VisualWater ${SOURCE_FILES})
target_link_libraries(VisualWater glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARY} glxw)