cmake_minimum_required(VERSION 3.9)
project(opencv_gui_test)


set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_ENABLE_EXPORTS ON)
set(CUDA_TOOLKIT_ROOT_DIR "/usr/local/cuda")

set(USE_CUDA OFF)
set(WITH_CUDA OFF)
find_package(PkgConfig REQUIRED)
#find_package(OpenCV REQUIRED)
find_package(CUDA 8.0 REQUIRED)
find_package(OpenCV 3.2.0 REQUIRED )
#pkg_search_module(OpenCV REQUIRED opencv)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}  main.cpp cvui.h )


target_link_libraries(${PROJECT_NAME}  ${OpenCV_LIBS})