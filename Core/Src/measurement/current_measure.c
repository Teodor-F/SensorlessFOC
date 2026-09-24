#include <current_measure.h>
#include <periph_layer_initializer.h>
#include <math.h>


static inline float_t calculate_current(const uint16_t adc_code, const float_t resolution, const float_t offset, const float_t current_gain)
{
	float_t ret_val = ((float_t)adc_code * resolution - offset) * current_gain;
	return ret_val;
}

void current_measure_init(current_measure_t *const instance, const current_measure_config_t* cfg)
{
	assert(instance != NULL && cfg != NULL);
	instance->adc_resolution = cfg->adc_ref_voltage / (float_t)cfg->adc_max_value;
	instance->inv_current_gain = (1.0f / (cfg->gain * cfg->shunt_value));
	instance->offset_voltage = cfg->offset_voltage;
}

void current_measure_process(current_measure_t *const instance)
{
	adc_currents_t adc_currs = adc_get_currents(adc);

	float_t voltage_offset = instance->offset_voltage;
	float_t curr_gain = instance->inv_current_gain;
	float_t resolution = instance->adc_resolution;

	instance->curr_a = calculate_current(adc_currs.curr_a, resolution, voltage_offset, curr_gain);
	instance->curr_b = calculate_current(adc_currs.curr_b, resolution, voltage_offset, curr_gain);
	instance->curr_c = calculate_current(adc_currs.curr_c, resolution, voltage_offset, curr_gain);
}

current_measure_act_curr_t current_measure_get_currents(const current_measure_t *const instance)
{
	current_measure_act_curr_t ret_val = {0};
	ret_val.curr_a = -instance->curr_a;
	ret_val.curr_b = -instance->curr_b;
	ret_val.curr_c = -instance->curr_c;
	return ret_val;
}
