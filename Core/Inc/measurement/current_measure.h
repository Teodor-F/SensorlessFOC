#ifndef CURRENT_MEASURE_H_
#define CURRENT_MEASURE_H_

#include <stdint.h>

typedef struct current_measure current_measure_t;
typedef struct current_measure_config current_measure_config_t;
typedef struct current_measure_act_curr current_measure_act_curr_t;


struct current_measure_config
{
	uint32_t adc_max_value;	// [no unit of measurement]
	float_t adc_ref_voltage;	// [mV]
	float_t gain;				// [no unit of measurement]
	float_t offset_voltage;	// [mV]
	float_t shunt_value;		// [ohm]
};

struct current_measure
{
	float_t adc_resolution;
	float_t inv_current_gain;
	float_t offset_voltage;

	float_t curr_a;		// [mA]
	float_t curr_b;		// [mA]
	float_t curr_c;		// [mA]

};

struct current_measure_act_curr {
	float_t curr_a;		// [mA]
	float_t curr_b;		// [mA]
	float_t curr_c;		// [mA]
};


void current_measure_init(current_measure_t *const instance, const current_measure_config_t* cfg);

void current_measure_process(current_measure_t *const instance);

current_measure_act_curr_t current_measure_get_currents(const current_measure_t *const instance);

#endif //  CURRENT_MEASURE_H_
