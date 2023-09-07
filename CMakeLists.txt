cmake_minimum_required (VERSION 3.8)

project(ECS CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Setup output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/bin") # .exe and .dll
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/lib") # .so and .dylib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/lib") # .lib and .a

add_library(ecs STATIC ${CMAKE_CURRENT_LIST_DIR}/src/ecs/ecs.cpp
${CMAKE_CURRENT_LIST_DIR}/src/ecs/pool.h
${CMAKE_CURRENT_LIST_DIR}/src/ecs/ecs.h
${CMAKE_CURRENT_LIST_DIR}/src/ecs/scope.h)

add_executable(example ${CMAKE_CURRENT_LIST_DIR}/src/examples/main.cpp)

target_link_libraries(example PRIVATE ecs)

target_include_directories(example PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)