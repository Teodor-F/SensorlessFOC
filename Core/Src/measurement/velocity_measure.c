#include <velocity_measure.h>
#include <numeric_constants.h>

void velocity_measure_init(velocity_measure_t *const instance, velocity_measure_config_t *cfg)
{
	assert(instance!= NULL);
	assert(cfg != NULL);
	instance->kv_value = cfg->kv_value;
	instance->pole_pairs = cfg->pole_pairs;
	instance->lpf_fo_instance = cfg->lpf_fo_instance;
	instance->rpm = 0.0f;
}

void velocity_measure_process(velocity_measure_t *const instance, float_t omega_electrical_rad_s)
{
	float_t velocitity_mechanical_rpm = (omega_electrical_rad_s * 60.0f) * (1.0f / (CONSTANT_TWO_PI * (float_t)instance->pole_pairs));
	if(instance->lpf_fo_instance != NULL)
	{
		lpf_first_order_process(instance->lpf_fo_instance, velocitity_mechanical_rpm);
	}
	instance->rpm = lpf_first_order_get_filtered_value(instance->lpf_fo_instance);
}

uint32_t velocity_measure_get_rpm(velocity_measure_t *const instance)
{
	uint32_t ret_val = (uint32_t)instance->rpm;
	return ret_val;
}

void velocity_measure_reset(velocity_measure_t *const instance)
{
	instance->rpm = 0.0f;
}

