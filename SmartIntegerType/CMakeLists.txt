cmake_minimum_required(VERSION 3.6)
project(SmartInteger)

#include_directories(boost_1_60_0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

set(SOURCE_FILES main.cpp InputWorkerStandartStream.cpp InputWorkerStandartStream.h InputWorkerFileStream.cpp InputWorkerFileStream.h SmartInteger.cpp SmartInteger.h OverflowChecks.h)
add_executable(SmartInteger ${SOURCE_FILES})