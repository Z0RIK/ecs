cmake_minimum_required (VERSION 3.8)

project(ECS CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Setup output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/bin") # .exe and .dll
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/lib") # .so and .dylib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/lib") # .lib and .a

add_library(ecs STATIC ${CMAKE_CURRENT_LIST_DIR}/src/ecs.cpp
${CMAKE_CURRENT_LIST_DIR}/src/pool.h
${CMAKE_CURRENT_LIST_DIR}/src/scope.h
${CMAKE_CURRENT_LIST_DIR}/src/scene.h)

target_include_directories(ecs 
PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src
PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

# building example
# raylib library
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/example/deps/raylib)

# raygui library
add_library(raygui INTERFACE ${CMAKE_CURRENT_LIST_DIR}/example/deps/raygui/src/raygui.h)
target_include_directories(raygui INTERFACE ${CMAKE_CURRENT_LIST_DIR}/example/deps/raygui/src)

# example executable
add_executable(example ${CMAKE_CURRENT_LIST_DIR}/example/src/main.cpp)

target_link_libraries(example 
PRIVATE ecs
PRIVATE raylib
PRIVATE raygui)