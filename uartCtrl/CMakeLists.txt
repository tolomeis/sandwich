add_executable(uartCtrl
	uartCtrl.c
)
pico_enable_stdio_usb(uartCtrl 1)
pico_add_extra_outputs(uartCtrl)

target_link_libraries(uartCtrl pico_stdlib hardware_uart hardware_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(uartCtrl)

# add url via pico_set_program_url
#example_auto_set_url(blink)
