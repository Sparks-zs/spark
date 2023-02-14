cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(spark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIRS})

add_subdirectory(src)