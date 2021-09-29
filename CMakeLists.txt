cmake_minimum_required(VERSION 3.7)
project(twophasepartitioner)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release'")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -fopenmp -mcx16 -DSTATS")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

find_package(Boost 1.62.0 REQUIRED)
find_package(gflags 2.1.2 REQUIRED)

add_library(converter converter/conversions.cpp converter/conversions.hpp converter/util.cpp converter/util.hpp converter/shuffler.cpp converter/shuffler.hpp)

add_executable(twophasepartitioner main.cpp globals.cpp twophasepartitioner.cpp stats.cpp streamcom.cpp)
add_executable(modularity modularity_calculator/main.cpp)
add_executable(dbh dbh/main.cpp dbh/dbh.cpp dbh/dbh.hpp dbh/globals.cpp dbh/globals.hpp dbh/stats.cpp dbh/stats.hpp)

target_link_libraries(twophasepartitioner converter glog gflags)
target_link_libraries(modularity gflags)
target_link_libraries(dbh converter glog gflags)