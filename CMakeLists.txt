cmake_minimum_required(VERSION 3.6)
project(rpifan)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c)
add_executable(rpifan ${SOURCE_FILES})

find_library(wiringPi_LIB wiringPi)
find_library(libconfig_LIB config)

target_link_libraries(rpifan ${wiringPi_LIB} ${libconfig_LIB})