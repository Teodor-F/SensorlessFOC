#ifndef CURRENT_MEASURE_H_
#define CURRENT_MEASURE_H_

#include <stdint.h>

typedef struct current_measure current_measure_t;
typedef struct current_measure_config current_measure_config_t;
typedef struct current_measure_act_curr current_measure_act_curr_t;
typedef struct current_measure_curr_alfa_beta current_measure_curr_alfa_beta_t;
typedef struct current_measure_curr_dq current_measure_curr_dq_t;

struct current_measure_config
{
	uint32_t adc_max_value;	// [no unit of measurement]
	float adc_ref_voltage;	// [mV]
	float gain;				// [no unit of measurement]
	float offset_voltage;	// [mV]
	float shunt_value;		// [ohm]
};

struct current_measure
{
	float adc_resolution;
	float inv_current_gain;
	float offset_voltage;

	float curr_a;		// [mA]
	float curr_b;		// [mA]
	float curr_c;		// [mA]
	float curr_alfa;	// [mA]
	float curr_beta;	// [mA]
	float curr_d;		// [mA]
	float curr_q;		// [mA]
};

struct current_measure_act_curr {
	float curr_a;		// [mA]
	float curr_b;		// [mA]
	float curr_c;		// [mA]
};

struct current_measure_curr_alfa_beta {
	float curr_alfa;	// [mA]
	float curr_beta;	// [mA]
};

struct current_measure_curr_dq
{
	float curr_d;		// [mA]
	float curr_q;		// [mA]
};


void current_measure_init(current_measure_t *const instance, const current_measure_config_t* cfg);

void current_measure_process(current_measure_t *const instance);

current_measure_act_curr_t current_measure_get_currents(const current_measure_t *const instance);

#endif //  CURRENT_MEASURE_H_
