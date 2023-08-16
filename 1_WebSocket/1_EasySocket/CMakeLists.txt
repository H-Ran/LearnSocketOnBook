cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR})
project(1_EasySocket LANGUAGES CXX)

add_executable(server server.cpp) 
add_executable(client client.cpp)