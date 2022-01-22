cmake_minimum_required(VERSION 3.10)

project(RemoteCounter)

add_executable(RemoteCounter main.cpp)

#configure_file(AppConfig.h.in AppConfig.h)

target_include_directories(RemoteCounter PUBLIC "${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
