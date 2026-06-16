cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_mpu_6309E C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico_mpu_6309E
    src/main.c
)

pico_generate_pio_header(pico_mpu_6309E
    ${CMAKE_CURRENT_LIST_DIR}/src/bus.pio
)

target_compile_options(pico_mpu_6309E PRIVATE
    -O3
)

target_compile_definitions(pico_mpu_6309E PRIVATE
    PICO_STDIO_DEFAULT_CRLF=0
    PICO_STDIO_USB_DEFAULT_CRLF=0
    PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=0
    PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE=0
    PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=0
)

target_link_libraries(pico_mpu_6309E PRIVATE
    pico_stdlib
    pico_multicore
    hardware_clocks
    hardware_pio
    hardware_vreg
)

pico_enable_stdio_usb(pico_mpu_6309E 1)
pico_enable_stdio_uart(pico_mpu_6309E 0)

pico_add_extra_outputs(pico_mpu_6309E)
