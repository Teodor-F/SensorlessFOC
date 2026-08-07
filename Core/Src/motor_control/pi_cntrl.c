#include <pi_cntrl.h>

#include <stddef.h>
#include <assert.h>

void pi_cntrl_init(pi_cntrl_t* const instance, const pi_cntrl_cfg_t *cfg)
{
	assert(instance != NULL && cfg != NULL);
	instance->kp = cfg->kp;
	instance->ki_ts = cfg->ki * cfg->ts;

	if (cfg->kp > 0.0f)
	{
	    instance->kt = cfg->ki / cfg->kp;
	}
	else
	{
	    instance->kt = 0.0f;
	}
	instance->out_max = cfg->out_limit;
	instance->out_min = -cfg->out_limit;
	instance->integral = 0.0f;
	instance->output = 0.0f;
}

void pi_cntrl_reset(pi_cntrl_t* const instance)
{
	assert(instance != NULL);
	instance->integral = 0.0f;
	instance->output = 0.0f;
}

float pi_cntrl_process(pi_cntrl_t* const instance, float error)
{
	assert(instance != NULL);

	// Calculate Unbounded Control Signal
	float out = instance->kp * error + instance->integral;
	float out_sat = out;

	// Apply Saturation
	if(out_sat > instance->out_max)
	{
		out_sat = instance->out_max;
	}
	else if (out_sat < instance->out_min)
	{
		out_sat = instance->out_min;
	}

	// Calculate Back-Calculation Feedback:
	instance->integral = (instance->integral + instance->ki_ts * error + instance->kt * (out_sat - out));

	if (instance->integral > instance->out_max)
	{
	    instance->integral = instance->out_max;
	}
	else if (instance->integral < instance->out_min)
	{
	    instance->integral = instance->out_min;
	}

	instance->output = out_sat;

	return out_sat;
}

