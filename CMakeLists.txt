cmake_minimum_required (VERSION 3.1)
project (traffic)
include_directories("src")
set (CMAKE_CXX_STANDARD 17)

# Set the name of the executable to be produced
set(EXECUTABLE_NAME traffic)

# Add all .cpp - files under src/ directory
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${EXECUTABLE_NAME} ${SOURCES})
set(CMAKE_BUILD_TYPE Debug)

# Detect and add SFML
# set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

# Detect and add SFML
set(SFML_STATIC_LIBRARIES TRUE)
#set(SFML_DIR "${CMAKE_SOURCE_DIR}/libs/SFML/cmake")
#option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

# SFML
#set(SFML sfml-graphics sfml-window sfml-system)
#add_subdirectory(libs/SFML)
#include_directories(${SFML_INCLUDE_DIR})
#link_libraries(${SFML})

#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
#find_package(SFML 2.5 REQUIRED network audio graphics window system)
#include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} sfml-network sfml-audio sfml-graphics sfml-window sfml-system)

# For installing targets (not mandatory)
install (TARGETS ${EXECUTABLE_NAME} DESTINATION bin)