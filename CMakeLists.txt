cmake_minimum_required(VERSION 3.12)

# +++hack+++ (should be given via -DPICO_SDK_PATH to cmake)
set(ENV{PICO_SDK_PATH} /home/pi/pico/pico-sdk)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(WS2812 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

pico_sdk_init()

add_compile_options(-Wall -Wextra
  -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
  -Wno-unused-function # we have some for the docs that aren't called
  -Wno-maybe-uninitialized
  -O3                 # must optimize: we rely on cycle-accurate timing as the time-sensitive protocol is bit-banged 
  )
add_executable(WS2812 WS2812.cpp)
target_link_libraries(WS2812 pico_stdlib)
pico_enable_stdio_usb(WS2812 0) # not using UART
pico_enable_stdio_uart(WS2812 0)
pico_add_extra_outputs(WS2812)
set_target_properties(WS2812 PROPERTIES ADDITIONAL_CLEAN_FILES "myPrj.uf2")
