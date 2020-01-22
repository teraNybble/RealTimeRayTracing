cmake_minimum_required(VERSION 3.15)
project(RealTimeRayTracing)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

file(GLOB SOURCE_FILES RealTimeRayTracing/*.cpp)

add_executable(RealTimeRayTracing ${SOURCE_FILES} RealTimeRayTracing/clHelp.h RealTimeRayTracing/clHelp.cpp RealTimeRayTracing/Engine.cpp RealTimeRayTracing/Engine.h RealTimeRayTracing/shaders/Shader.cpp RealTimeRayTracing/Cube.cpp RealTimeRayTracing/Pos3.cpp RealTimeRayTracing/main.cpp)

INCLUDE(FindPkgConfig)

find_package(OpenCL REQUIRED)
find_package(X11 REQUIRED)
find_package(Threads)
find_package(OpenGL REQUIRED)
find_package(glfw REQUIRED)#was glfw3
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)

include_directories(${OpenCL_INCLUDE_DIR})
include_directories(${X11_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
#target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${GLFW_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARIES})