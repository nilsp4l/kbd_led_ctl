option(ALL_DEVICES              "Enable every device"      OFF)
option(DEVICE_IT5570E           "Enable IT5570E"           OFF)
option(DEVICE_ROCCAT_VULCAN_TKL "Enable ROCCAT Vulcan TKL" OFF)

if (DEVICE_IT5570E OR ALL_DEVICES)
  target_sources(${EXECUTABLE_NAME} PRIVATE it5570e.cpp)
  target_compile_definitions(${EXECUTABLE_NAME} PRIVATE DEVICE_IT5570E=1)
endif()

if(DEVICE_ROCCAT_VULCAN_TKL OR ALL_DEVICES)
  target_sources(${EXECUTABLE_NAME} PRIVATE roccat_vulcan_tkl.cpp)
  target_compile_definitions(${EXECUTABLE_NAME} PRIVATE DEVICE_ROCCAT_VULCAN_TKL=1)
endif()
