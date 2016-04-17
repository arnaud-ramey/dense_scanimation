PROJECT(dense_scanimation CXX)
cmake_minimum_required(VERSION 2.8)
# Debug, Release, RelWithDebInfo and MinSizeRe
set(CMAKE_BUILD_TYPE RelWithDebInfo)
SET(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra") # add extra warnings

FIND_PACKAGE(OpenCV)

ADD_EXECUTABLE(dense_scanimation_maker.exe dense_scanimation_maker.cpp)
TARGET_LINK_LIBRARIES(dense_scanimation_maker.exe ${OpenCV_LIBS})
