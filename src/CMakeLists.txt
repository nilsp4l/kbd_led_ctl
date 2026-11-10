
set(EXECUTABLE_NAME kbd_led_ctl)


add_executable(${EXECUTABLE_NAME} main.cpp)
target_sources(${EXECUTABLE_NAME} PRIVATE rgbi.cpp)
add_subdirectory(device)
add_subdirectory(hid)
add_subdirectory(posix)


target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)


add_library(warning_flags INTERFACE )
target_compile_options(warning_flags INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wconversion;-pedantic;-Wfatal-errors>)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE warning_flags)

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(${EXECUTABLE_NAME} PRIVATE  $<$<CONFIG:Release>:-O2>)

