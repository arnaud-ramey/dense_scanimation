PROJECT(dense_scanimation CXX)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_BUILD_TYPE             RelWithDebInfo)
FIND_PACKAGE(OpenCV)

ADD_EXECUTABLE(dense_scanimation_maker.exe dense_scanimation_maker.cpp)
TARGET_LINK_LIBRARIES(dense_scanimation_maker.exe ${OpenCV_LIBS})
