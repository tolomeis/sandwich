cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(uartCtrl C CXX ASM)

set(CMAKE_C_STANDARD 11)
Set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(uartCtrl
	uartCtrl.c
)
pico_enable_stdio_usb(uartCtrl 1)
pico_add_extra_outputs(uartCtrl)

target_link_libraries(uartCtrl pico_stdlib hardware_uart hardware_pwm)

