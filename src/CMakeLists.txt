
set(EXECUTABLE_NAME kbd_led_ctl)


add_executable(${EXECUTABLE_NAME} main.cpp)

add_subdirectory(devices)
add_subdirectory(hid)
add_subdirectory(posix)

target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
