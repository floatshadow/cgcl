cmake_minimum_required(VERSION 3.11)
project(cgcl VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CGCL_BUILD_TEST ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenMP REQUIRED)

set(COMMON_INCLUDES ${CMAKE_SOURCE_DIR}/include ${GLM_INCLUDE_DIRS})
include_directories(${COMMON_INCLUDES})

if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-DNEED_LOG_STACK_TRACE=1 -rdynamic -g)
endif()

add_subdirectory(lib)
if (CGCL_BUILD_TEST)
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/test)
endif()