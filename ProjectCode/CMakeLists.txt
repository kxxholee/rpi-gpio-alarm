cmake_minimum_required(VERSION 3.0)
project(TermProject17)

# VSCode의 C include path에 추가
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++11 표준 설정
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenCV 찾기
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# TensorFlow Lite 헤더와 라이브러리 경로
set(TFLITE_INCLUDE_DIR /home/pi/working/ProjectCode/CTesting/tensorflow)
set(TFLITE_LIB_DIR /home/pi/working/ProjectCode/CTesting/tensorflow/tensorflow/lite/tools/make/gen/rpi_armv7l/lib)

include_directories(${TFLITE_INCLUDE_DIR})
link_directories(${TFLITE_LIB_DIR})

# 소스 파일 정의
set(SOURCE_FILES main.cpp)

# 실행 파일 추가
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# 링크 라이브러리
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} tensorflow-lite pthread dl)
