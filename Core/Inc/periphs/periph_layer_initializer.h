#ifndef PERIPH_LAYER_INITIALIZER_H_
#define PERIPH_LAYER_INITIALIZER_H_

#include <adc.h>
#include <pwm.h>
#include <mctimer.h>
#include <uart.h>

extern adc_t *adc;
extern pwm_t *pwm;
extern mc_timer_t *mc_timer;
extern uart_t *uart;


void periphs_layer_initializer(void);


#endif /* PERIPH_LAYER_INITIALIZER_H_ */
