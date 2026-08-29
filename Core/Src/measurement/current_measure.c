#include <current_measure.h>
#include <adc.h>
#include <config/motor_cfg.h>
#include <pwm.h>


#include <math.h>


static inline float calculate_current(const uint16_t adc_code, const float resolution, const float offset, const float current_gain)
{
	float ret_val = ((float)adc_code * resolution - offset) * current_gain;
	return ret_val;
}

void current_measure_init(current_measure_t *const instance, const current_measure_config_t* cfg)
{
	assert(instance != NULL && cfg != NULL);
	instance->adc_resolution = cfg->adc_ref_voltage / (float)cfg->adc_max_value;
	instance->inv_current_gain = (1.0f / (cfg->gain * cfg->shunt_value));
	instance->offset_voltage = cfg->offset_voltage;
}

void current_measure_process(current_measure_t *const instance)
{
	adc_currents_t adc_currs = adc_get_currents();

	float voltage_offset = instance->offset_voltage;
	float curr_gain = instance->inv_current_gain;
	float resolution = instance->adc_resolution;

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
