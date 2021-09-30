
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

// **** UART CONFIG ***** // 
#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

#define PWM_WRAP 2000 // freq = 50Khz 

// ***** PIN CONFIG ***** //
#define UART_TX_PIN 8
#define UART_RX_PIN 5

#define AIN1	11
#define AIN2	10
#define PWMA	9
#define BIN1	15
#define BIN2	14
#define PWMB	13
#define stby	12

#define LED_PIN	25
#define STARTM 6


int vdx, vsx = 0;
uint8_t reicval[2];
int ledval = 0;
int velA, velB = 0;

// RX interrupt handler
void on_uart_rx() {
	while (uart_is_readable(UART_ID)) {
		uint8_t ch = uart_getc(UART_ID);
		if((ch == 's') && (gpio_get(STARTM))){
			uart_read_blocking(UART_ID, reicval, 2);
			
			vdx = (int8_t) reicval[0];
			vsx = (int8_t) reicval[1];
			ledval = !ledval;
			gpio_put(LED_PIN,ledval);
			vdx = vdx>100? 0:vdx;
			vdx = vdx<-100? 0:vdx;
			vsx = vsx>100? 0:vsx;
			vsx = vsx<-100? 0:vsx;

			velA = vdx>=0?vdx*(PWM_WRAP/100):-vdx*(PWM_WRAP/100);
			velB = vsx>=0?vsx*(PWM_WRAP/100):-vsx*(PWM_WRAP/100);
			pwm_set_gpio_level(PWMA,velA);
			pwm_set_gpio_level(PWMB,velB);

			gpio_put(AIN1, vdx>0);
			gpio_put(AIN2, vdx<0);
			gpio_put(BIN1, vsx>0);
			gpio_put(BIN2, vsx<0);
			gpio_put(stby, vsx!=0);
		} else {
			pwm_set_gpio_level(PWMA,0);
			pwm_set_gpio_level(PWMB,0);
			uart_puts(UART_ID, "[WARN] Random stuff recieved\n");
		}

	}	
}

int main() {

	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	gpio_put(LED_PIN,ledval);

	gpio_init(STARTM);
	gpio_set_dir(STARTM, GPIO_IN);
	gpio_pull_down(STARTM);

	uart_init(UART_ID, 115200);

	// Set the TX and RX pins by using the function select on the GPIO
	gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

	// Set baudrate and get the actual speed
	int actual = uart_set_baudrate(UART_ID, BAUD_RATE);

	//Set UART flow control CTS/RTS, we don't want these, so turn them off
	uart_set_hw_flow(UART_ID, false, false);

	// Turn off FIFO's - we want to do this character by character
	uart_set_fifo_enabled(UART_ID, false);

	// Select correct interrupt for the UART we are using
	int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

	// And set up and enable the interrupt handlers
	irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
	irq_set_enabled(UART_IRQ, true);

	// Now enable the UART to send interrupts - RX only
	uart_set_irq_enables(UART_ID, true, false);

	// OK, all set up.
	uart_puts(UART_ID, "Connected\n");

	
	gpio_init(AIN1);
	gpio_set_dir(AIN1, GPIO_OUT);
	gpio_put(AIN1, 0);

	gpio_init(BIN1);
	gpio_set_dir(BIN1, GPIO_OUT);
	gpio_put(BIN1, 1);

	gpio_init(AIN2);
	gpio_set_dir(AIN2, GPIO_OUT);
	gpio_put(AIN2, 0);

	gpio_init(BIN2);
	gpio_set_dir(BIN2, GPIO_OUT);
	gpio_put(BIN2, 1);
	
	gpio_init(stby);
	gpio_set_dir(stby, GPIO_OUT);
	gpio_put(stby, 0);



	gpio_set_function(PWMA, GPIO_FUNC_PWM);
	gpio_set_function(PWMB, GPIO_FUNC_PWM);

	uint slice0 = pwm_gpio_to_slice_num(PWMA);
	uint slice1 = pwm_gpio_to_slice_num(PWMB);

	pwm_set_clkdiv_mode(slice0, PWM_DIV_FREE_RUNNING);
	pwm_set_phase_correct(slice0, false);
	pwm_set_clkdiv_int_frac(slice0, 1, 4);
	pwm_set_wrap(slice0, PWM_WRAP);		// Freq of about 50KHz 

	pwm_set_clkdiv_mode(slice1, PWM_DIV_FREE_RUNNING);
	pwm_set_phase_correct(slice1, false);
	pwm_set_clkdiv_int_frac(slice1, 1, 4);
	pwm_set_wrap(slice1, PWM_WRAP);

	//pwm_set_output_polarity(slice0, false, true);
	//uint slice1 = pwm_gpio_to_slice_num(PWMB);
	
	pwm_set_gpio_level (PWMA, 0);
	pwm_set_gpio_level (PWMB, 0);
	pwm_set_enabled(slice0, true);
	pwm_set_enabled(slice1, true);
	
	while (1)
		tight_loop_contents();
}