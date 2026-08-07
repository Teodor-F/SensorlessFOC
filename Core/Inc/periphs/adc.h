#ifndef ADC_H
#define ADC_H

#include <stdint.h>

typedef struct adc_currents	adc_currents_t;

typedef enum adc_signal adc_signal_t;

struct adc_currents
{
	uint16_t curr_a;
	uint16_t curr_b;
	uint16_t curr_c;
};

enum adc_signal {
	VBUS,
	TEMP,
	BEMF_A,
	BEMF_B,
	BEMF_C
};

void adc_init(void);

adc_currents_t adc_get_currents(void);

uint16_t adc_get_signal(adc_signal_t adc_signal);

#endif /* ADC_H */
