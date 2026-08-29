#include <current_controller.h>

void current_controller_init(current_controller_t* const instance, const current_controller_cfg_t *cfg)
{
	assert(instance != NULL);

	pi_controller_cfg_t pi_id_config = {
		.kp = cfg->kp_id,
		.ki = cfg->ki_id,
		.out_limit = cfg->vd_out_limit,
		.ts = cfg->ts
	};
	pi_controller_init(&instance->id_pi_cntrl, &pi_id_config);

	pi_controller_cfg_t pi_iq_config = {
		.kp = cfg->kp_iq,
		.ki = cfg->ki_iq,
		.out_limit = cfg->vq_out_limit,
		.ts = cfg->ts
	};
	pi_controller_init(&instance->iq_pi_cntrl, &pi_iq_config);

	instance->ts = cfg->ts;
	instance->id_ref = 0.0f;
	instance->iq_ref = 0.0f;
	instance->vd_out = 0.0f;
	instance->vq_out = 0.0f;

}

void current_controller_process(current_controller_t* const instance, float id_current_actual, float iq_current_actual)
{
	float id_error = instance->id_ref - id_current_actual;
	instance->vd_out = pi_controller_process(&instance->id_pi_cntrl, id_error);

	float iq_error = instance->iq_ref - iq_current_actual;
	instance->vq_out = pi_controller_process(&instance->iq_pi_cntrl, iq_error);
}

void current_controller_set_target_id(current_controller_t* const instance, float new_id)
{
	instance->id_ref = new_id;
}

void current_controller_set_target_iq(current_controller_t* const instance, float new_iq)
{
	instance->iq_ref = new_iq;
}

float current_controller_get_target_id(current_controller_t* const instance)
{
	float ret_val = instance->id_ref;
	return ret_val;
}

float current_controller_get_target_iq(current_controller_t* const instance)
{
	float ret_val = instance->iq_ref;
	return ret_val;
}

current_controller_output_t current_controller_get_vd_vq_out(current_controller_t* const instance)
{
	current_controller_output_t ret_val;
	ret_val.vd = instance->vd_out;
	ret_val.vq = instance->vq_out;
	return ret_val;
}

void current_controller_reset(current_controller_t* const instance)
{
	pi_controller_reset(&instance->id_pi_cntrl);
	pi_controller_reset(&instance->iq_pi_cntrl);
}
