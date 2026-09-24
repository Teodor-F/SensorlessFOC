#include <lpf_first_order.h>
#include <assert.h>

void lpf_first_order_init(lpf_first_order_t *const instance, lpf_first_order_cfg_t* cfg)
{
	assert(instance != NULL);
	assert(cfg != NULL);

	instance->cfg = *cfg;
	instance->sampling_time = ((float_t)instance->cfg.cycle_time / CYCLE_TIME_DIVIDER);
	instance->tau = 1.0f / (CONSTANT_TWO_PI * instance->cfg.cutoff_freq_hz);
	instance->lpf_constant = instance->sampling_time / (instance->tau + instance->sampling_time);
	instance->filtered_value = 0.0f;
}

void lpf_first_order_process(lpf_first_order_t *const instance, float_t unfiltered_value)
{
	float_t temp_value = instance->filtered_value * (1.0f - instance->lpf_constant) + (unfiltered_value * instance->lpf_constant);
	instance->filtered_value = temp_value;
}

float_t lpf_first_order_get_filtered_value(lpf_first_order_t *const instance)
{
	float_t ret_val = instance->filtered_value;
	return ret_val;
}

