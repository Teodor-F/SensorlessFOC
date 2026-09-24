#include <revup_controller.h>
#include <motor_cfg.h>

static inline float_t constrain_angle(float_t angle)
{
	float_t ret_val = angle;
	if(fabsf(angle) >= CONSTANT_TWO_PI)
	{
		ret_val = 0.0f;
	}
	return ret_val;
}

void revup_controller_init(revup_controller_t *const instance, const revup_controller_config_t *cfg)
{
	assert(instance != NULL);

	instance->revup_cfg = *cfg;
	instance->revup_cntrl_state = REVUP_CONTROLLER_IDLE;
	instance->revup_finished = FALSE;


	instance->sampling_time = ((float_t)cfg->cycle_time / CYCLE_TIME_DIVIDER);


	instance->aligment_ticks = (uint32_t)(((float_t)instance->revup_cfg.alignment_time * (1.0f/instance->sampling_time)) / 1000.0f);
	instance->aligment_tick_counter = 0u;
	instance->open_loop_ramp_time_ticks = (uint32_t)(((float_t)instance->revup_cfg.open_loop_ramp_time * (1.0f/instance->sampling_time)) / 1000.0f);
	instance->open_loop_ramp_time_tick_counter = 0u;

	uint32_t pole_pairs = motor_cfg_get_motor_pole_pairs();
	float_t temp_elec_freq_hz = ((float_t)((instance->revup_cfg.open_loop_velocity_setpoint * pole_pairs * 2u) / 120.0f));
	instance->open_loop_omega_max = temp_elec_freq_hz * CONSTANT_TWO_PI;
	instance->open_loop_omega_dt = (instance->open_loop_omega_max / (instance->revup_cfg.open_loop_ramp_time / 1000.0f));

	instance->stabilization_time_ticks = (uint32_t)(((float_t)instance->revup_cfg.stabilization_time * (1.0f/instance->sampling_time)) / 1000.0f);
	instance->stabilization_time_tick_counter = 0u;

	instance->theta_out = 0.0f;
	instance->id_out = 0.0f;
	instance->iq_out = 0.0f;

}

void revup_controller_process(revup_controller_t *const instance)
{
	switch(instance->revup_cntrl_state)
	{
//========================================================================
	// Align the motor to a predefined angle
		case REVUP_CONTROLLER_ALIGMENT:
		{
			instance->theta_out = 0.0f;
			instance->id_out = instance->revup_cfg.aligment_id;
			instance->iq_out = instance->revup_cfg.aligment_iq;
			instance->aligment_tick_counter++;
			if(instance->aligment_tick_counter >= instance->aligment_ticks)
			{
				instance->aligment_tick_counter = 0u;
				instance->revup_cntrl_state = REVUP_CONTROLLER_OPEN_LOOP_ACCELERATION;
			}
			break;
		}
//========================================================================
		// Accelerate the motor in open-loop mode
		case REVUP_CONTROLLER_OPEN_LOOP_ACCELERATION:
		{
			instance->open_loop_omega += (instance->open_loop_omega_dt * instance->sampling_time);
			if(instance->open_loop_omega >= instance->open_loop_omega_max)
			{
				instance->open_loop_omega = instance->open_loop_omega_max;
				instance->revup_cntrl_state = REVUP_CONTROLLER_STABILIZATION;
			}
			instance->theta_out += (instance->open_loop_omega * instance->sampling_time);
			instance->theta_out = constrain_angle(instance->theta_out);
			instance->id_out = instance->revup_cfg.aligment_id;
			instance->iq_out = instance->revup_cfg.aligment_iq;
			break;
		}
//========================================================================
		// Spin the motor with the s
		case REVUP_CONTROLLER_STABILIZATION:
		{

			instance->theta_out += (instance->open_loop_omega * instance->sampling_time);
			instance->theta_out = constrain_angle(instance->theta_out);
			instance->id_out = instance->revup_cfg.stabilization_id;
			instance->iq_out = instance->revup_cfg.stabilization_iq;

			instance->stabilization_time_tick_counter++;
			if(instance->stabilization_time_tick_counter >= instance->stabilization_time_ticks)
			{
				instance->stabilization_time_tick_counter = 0u;
				instance->id_out = 0.0f;
				instance->iq_out = instance->revup_cfg.stabilization_iq;
				instance->revup_finished = TRUE;
				instance->revup_cntrl_state = REVUP_CONTROLLER_IDLE;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void revup_controller_reset(revup_controller_t *const instance)
{
	instance->revup_cntrl_state = REVUP_CONTROLLER_IDLE;
	instance->revup_finished = FALSE;
	instance->aligment_tick_counter = 0u;
	instance->open_loop_omega = 0u;
	instance->open_loop_ramp_time_tick_counter = 0u;
	instance->stabilization_time_tick_counter = 0u;
	instance->theta_out = 0.0f;
	instance->id_out = 0.0f;
	instance->iq_out = 0.0f;
}

revup_controller_current_output_t revup_controller_get_output(revup_controller_t *const instance)
{
	revup_controller_current_output_t ret_val =
	{
		.id_ref = instance->id_out,
		.iq_ref = instance->iq_out,
	};
	return ret_val;
}


float_t revup_controller_get_open_loop_electrical_angle(revup_controller_t *const instance)
{
	float_t ret_val = instance->theta_out;
	return ret_val;
}

float_t revup_controller_get_stabilization_q_current(revup_controller_t *const instance)
{
	float_t ret_val = instance->revup_cfg.stabilization_iq;
	return ret_val;
}

uint32_t revup_controller_get_open_loop_velocity_setpoint(revup_controller_t *const instance)
{
	uint32_t ret_val = instance->revup_cfg.open_loop_velocity_setpoint;
	return ret_val;
}

bool_t revup_controller_activate(revup_controller_t *const instance)
{
	bool_t ret_val = FALSE;

	if(instance->revup_cntrl_state == REVUP_CONTROLLER_IDLE)
	{
		instance->revup_cntrl_state = REVUP_CONTROLLER_ALIGMENT;
		ret_val = TRUE;
	}
	else
	{
		instance->revup_cntrl_state = REVUP_CONTROLLER_IDLE;
	}
	return ret_val;
}


bool_t revup_controller_is_finished(revup_controller_t *const instance)
{
	bool_t ret_val = instance->revup_finished;
	return ret_val;
}
