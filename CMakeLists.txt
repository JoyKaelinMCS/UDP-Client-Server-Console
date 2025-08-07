cmake_minimum_required(VERSION 3.10)
project(UDP-CLIENT-SERVER-CONSOLE)

set(CMAKE_CXX_STANDARD 20)

add_executable(server server.cpp)
add_executable(client client.cpp)
