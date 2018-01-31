cmake_minimum_required(VERSION 3.0)
project(maths)

set(EXECUTABLE_NAME "maths")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb3 -g")

file(GLOB SOURCES "src/*.cpp")
add_executable(${EXECUTABLE_NAME} ${SOURCES})

find_package(SFML 2 REQUIRED audio graphics window system)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
