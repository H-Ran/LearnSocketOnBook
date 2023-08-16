cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR})
project(blocking LANGUAGES CXX)


add_executable(nonblocking_server nonblocking_server.cpp)
add_executable(nonblocking_client_send nonblocking_client_send.cpp)
add_executable(nonblocking_client_recv nonblocking_client_recv.cpp)
add_executable(nonblocking_client_connect nonblocking_client_connect.cpp)