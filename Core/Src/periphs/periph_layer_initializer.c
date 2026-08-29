#include <config/motor_cfg.h>
#include <periph_layer_initializer.h>

void periphs_layer_initializer(void)
{
//========================================================================
	// ADC initialization
	adc_init();

//========================================================================
	// PWM-signal generator initialization
	pwm_init();

//========================================================================
	// Motor control timer initialization
	mc_timer_init();
}
