cmake_minimum_required(VERSION 3.23)
project(OpenCVStudy)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(OpenCVStudy 3.图像绘制与模糊.cpp)

target_link_libraries(OpenCVStudy ${OpenCV_LIBS})
