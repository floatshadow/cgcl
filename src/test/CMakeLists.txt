set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_executable(OBJParser OBJParser.cpp)
target_link_libraries(OBJParser ${PROJECT_NAME})