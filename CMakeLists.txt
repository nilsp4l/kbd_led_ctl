cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Release)

project(kbd_led_ctl)

add_compile_options(-Wall -Wextra -Wconversion -pedantic -Wfatal-errors -O2)


add_subdirectory(src)
