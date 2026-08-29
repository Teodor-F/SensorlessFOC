#include <config/motor_cfg.h>
#include <pi_controller.h>
#include <numeric_constants.h>

void pi_controller_init(pi_controller_t* const instance, const pi_controller_cfg_t* const cfg)
{
	instance->kp = cfg->kp;
	instance->ki = cfg->ki;
	instance->ts = cfg->ts;
	instance->ki_ts_by_two = ONE_BY_TWO * instance->ki * instance->ts;
	instance->output_max = cfg->out_limit;
	instance->output_min = -cfg->out_limit;
	instance->integral = 0.0f;
	instance->prev_error = 0.0f;
	instance->output = 0.0f;
}

float pi_controller_process(pi_controller_t* const instance, float error)
{
	float ret_val = 0.0f;

	float p_term = instance->kp * error;
	float i_term_temp = instance->integral +instance->ki_ts_by_two * (error + instance->prev_error);
	float output = instance->kp * error + i_term_temp;

	if(output < instance->output_max &&	output > instance->output_min)
	{
		instance->integral = i_term_temp;
	}

	instance->output = p_term + instance->integral;
	if (instance->output > instance->output_max)
	{
		instance->output = instance->output_max;
	}
	else if (instance->output < instance->output_min)
	{
		instance->output = instance->output_min;
	}
	instance->prev_error = error;

	ret_val = instance->output;
	return ret_val;
}

void pi_controller_reset(pi_controller_t* const instance)
{
	instance->output = 0.0f;
	instance->integral = 0.0f;
	instance->prev_error = 0.0f;
}
