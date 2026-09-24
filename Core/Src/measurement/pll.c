#include <pll.h>
#include <assert.h>
#include <config/motor_cfg.h>
#include <numeric_constants.h>

void pll_init(pll_t* const instance, const pll_cfg_t* cfg)
{
	assert(instance != NULL && cfg != NULL);
	instance->kp = cfg->kp;
	instance->ki = cfg->ki;
	instance->ts = cfg->ts;
	instance->ki_ts = instance->ki * instance->ts;
	instance->omega_max = cfg->omega_max;
	instance->omega_est = 0.0f;
	instance->omega_integral = 0.0f;
	instance->theta_est = 0.0f;
}

void pll_process(pll_t* const instance, float_t emf_alpha_est, float_t emf_beta_est)
{
    assert(instance != NULL);

	float_t sin_value = sinf(instance->theta_est);
	float_t cos_value = cosf(instance->theta_est);

    // normalize EMF to remove amplitude dependency
    float_t mag = fabsf(emf_alpha_est) + fabsf(emf_beta_est) + 1e-6f;

    float_t error = (emf_alpha_est * sin_value - emf_beta_est * cos_value) / mag;

    // PI controller
    instance->omega_est += error * instance->ki;

    // clamp omega_est
    if (instance->omega_est > instance->omega_max)
    {
        instance->omega_est = instance->omega_max;
    }
    else if (instance->omega_est < -instance->omega_max)
    {
        instance->omega_est = -instance->omega_max;
    }

    float_t omega_out = instance->omega_est + instance->kp * error;

    // optional clamp (extra safety)
    if (omega_out > instance->omega_max)
    {
        omega_out = instance->omega_max;
    }
    else if (omega_out < -instance->omega_max)
    {
        omega_out = -instance->omega_max;
    }

    // integrate angle
    instance->theta_est += omega_out * instance->ts;

    // wrap
    if (instance->theta_est >= CONSTANT_TWO_PI)
    {
        instance->theta_est -= CONSTANT_TWO_PI;
    }
    else if (instance->theta_est < 0.0f)
    {
        instance->theta_est += CONSTANT_TWO_PI;
    }
}

void pll_process_new(pll_t* const instance, float_t emf_alpha_est, float_t emf_beta_est)
{
    assert(instance != NULL);

	float_t sin_value = sinf(instance->theta_est);
	float_t cos_value = cosf(instance->theta_est);

	float_t mag = sqrtf(emf_alpha_est * emf_alpha_est + emf_beta_est * emf_beta_est) + 1e-6f;
	float_t error = (-emf_alpha_est * sin_value +  emf_beta_est * cos_value) / mag;

	// PI-block
	instance->omega_integral = instance->omega_integral + instance->ki_ts * error;
	if(instance->omega_integral > instance->omega_max)
	{
		instance->omega_integral = instance->omega_max;
	}
	else if(instance->omega_integral < -instance->omega_max)
	{
		instance->omega_integral = -instance->omega_max;
	}

	float_t omega = instance->kp * error + instance->omega_integral;
	if(omega > instance->omega_max)
	{
		omega = instance->omega_max;
	}
	else if (omega < -instance->omega_max)
	{
		omega = -instance->omega_max;
	}
	instance->omega_est = omega;


	// Integrate omega to obtain the theta
	instance->theta_est = instance->theta_est + instance->omega_est * instance->ts;
    // wrap
    if (instance->theta_est > CONSTANT_TWO_PI)
    {
        instance->theta_est -= CONSTANT_TWO_PI;
    }
    else if (instance->theta_est < 0.0f)
    {
        instance->theta_est += CONSTANT_TWO_PI;
    }
}


void pll_reset(pll_t* const instance)
{
	assert(instance != NULL);
	instance->omega_integral = 0.0f;
	instance->omega_est = 0.0f;
	instance->theta_est = 0.0f;
}


float_t pll_get_est_theta(const pll_t* const instance)
{
	assert(instance != NULL);

	float_t temp = instance->theta_est;
	temp -= CONSTANT_PI * ONE_BY_TWO;
	CONSTRAIN_ANGLE_RAD_ZERO_TWO_PI(temp);
	float_t ret_val = temp;
	return ret_val;
}

float_t pll_get_est_omega(const pll_t* const instance)
{
	assert(instance != NULL);
	float_t ret_val = instance->omega_est;
	return ret_val;
}

