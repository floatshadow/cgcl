cmake_minimum_required(VERSION 3.11)
project(cgcl VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenMP REQUIRED)

set(COMMON_INCLUDES ${CMAKE_SOURCE_DIR}/include ${GLM_INCLUDE_DIRS})
include_directories(${COMMON_INCLUDES})

add_subdirectory(lib)