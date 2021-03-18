cmake_minimum_required(VERSION 3.17)
project(SerialPortTest)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-lCppLinuxSerial)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE CppLinuxSerial.so)
target_include_directories(${PROJECT_NAME} PRIVATE SSCP/)
target_precompile_headers(${PROJECT_NAME} PRIVATE SSCP/SerialConnection.h)