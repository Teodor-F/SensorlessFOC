#include <periph_layer_initializer.h>

adc_t *adc = NULL;
static adc_t adc_instance;

pwm_t *pwm = NULL;
static pwm_t pwm_instance;

mc_timer_t *mc_timer = NULL;
static mc_timer_t mc_timer_instance;

uart_t *uart = NULL;
static uart_t uart_instance;

void periphs_layer_initializer(void)
{
//========================================================================
	// ADC initialization
	adc_init(&adc_instance);
	adc = &adc_instance;

//========================================================================
	// PWM-signal generator initialization
	pwm_init(&pwm_instance);
	pwm = &pwm_instance;

//========================================================================
	// Motor control timer initialization
	mc_timer_init(&mc_timer_instance);
	mc_timer = &mc_timer_instance;

//========================================================================
	// UART initialization
	uart_init(&uart_instance);
	uart = &uart_instance;
}

