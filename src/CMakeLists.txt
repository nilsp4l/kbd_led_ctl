
set(EXECUTABLE_NAME kbd_led_ctl)


add_executable(${EXECUTABLE_NAME} main.cpp)
target_sources(${EXECUTABLE_NAME} PRIVATE rgbi.cpp)
add_subdirectory(device)
add_subdirectory(hid)
add_subdirectory(posix)

target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
