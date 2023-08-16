cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR})
project(blocking LANGUAGES CXX)


add_executable(blocking_server blocking_server.cpp)
add_executable(blocking_client blocking_client.cpp)