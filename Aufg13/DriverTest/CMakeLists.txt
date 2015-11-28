cmake_minimum_required(VERSION 2.8)

project(drivertest)

add_executable(drivertest main.cpp)

include_directories(/home/ntb/zoom/buildroot-toolchain/buildroot-toolchain-HS14/include)

set(CMAKE_CXX_FLAGS "-std=c++11")


install(TARGETS drivertest RUNTIME DESTINATION bin)