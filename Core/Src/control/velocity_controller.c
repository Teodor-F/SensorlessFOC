#include <velocity_controller.h>


void velocity_controller_init(velocity_controller_t *const instance, const velocity_controller_cfg_t *cfg)
{
	pi_controller_cfg_t pi_velocity_cfg = {
		.kp = cfg->kp,
		.ki = cfg->ki,
		.out_limit = cfg->iq_max_out,
		.ts = cfg->ts
	};
	pi_controller_init(&instance->velocity_pi_cntrl, &pi_velocity_cfg);

	instance->velocity_ref = 0.0f;
	instance->ts = cfg->ts;
	instance->iq_out = 0.0f;
}

void velocity_controller_process(velocity_controller_t *const instance, float_t velocity_actual)
{
	float_t velocity_error = instance->velocity_ref - velocity_actual;
	instance->iq_out = pi_controller_process(&instance->velocity_pi_cntrl, velocity_error);
}

void velocity_controller_set_target_velocity(velocity_controller_t *const instance, float_t new_velocity)
{
	instance->velocity_ref = new_velocity;
}

float_t velocity_controller_get_target_velocity(velocity_controller_t *const instance)
{
	float_t ret_val = instance->velocity_ref;
	return ret_val;
}

float_t velocity_controller_get_current_out(velocity_controller_t *const instance)
{
	float_t ret_val = instance->iq_out;
	return ret_val;
}

void velocity_controller_reset(velocity_controller_t *const instance)
{
	instance->iq_out = 0.0f;
	instance->velocity_ref = 0.0f;
	pi_controller_reset(&instance->velocity_pi_cntrl);
}
