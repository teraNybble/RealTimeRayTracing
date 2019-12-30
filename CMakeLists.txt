cmake_minimum_required(VERSION 3.15)
project(RealTimeRayTracing)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

file(GLOB SOURCE_FILES RealTimeRayTracing/*.cpp)

add_executable(RealTimeRayTracing ${SOURCE_FILES})

INCLUDE(FindPkgConfig)

find_package(OpenCL REQUIRED)

include_directories(${OpenCL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARIES})