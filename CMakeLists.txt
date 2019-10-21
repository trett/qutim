cmake_minimum_required(VERSION 3.13)

project(qutim)

cmake_policy(SET CMP0079 NEW)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -fstrict-aliasing -Wno-deprecated-declarations -Wno-unused-variable")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src)
#add_subdirectory(artwork)
#add_subdirectory(translations)


