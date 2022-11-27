set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(GLOB LIBSRC ./*/*.cpp)
file(GLOB OPENGL_SRC ./platform/OpenGL/*)

add_library(${CMAKE_PROJECT_NAME} ${LIBSRC} ${OPENGL_SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenMP::OpenMP_CXX OpenGL::GL)