#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define ADC_CURRENTS_NUM 	3u

typedef struct adc_currents	adc_currents_t;
typedef struct adc adc_t;
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
};

struct adc {
	ADC_HandleTypeDef adc_periph_1;
	ADC_HandleTypeDef adc_periph_2;
	uint16_t *adc_currents[ADC_CURRENTS_NUM];
};


void adc_init(adc_t *const instance);

adc_currents_t adc_get_currents(adc_t *const instance);

uint16_t adc_get_signal(adc_t *const instance, adc_signal_t adc_signal);

#endif /* ADC_H */
