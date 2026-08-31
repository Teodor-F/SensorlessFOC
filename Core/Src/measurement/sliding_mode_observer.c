#include <sliding_mode_observer.h>
#include <assert.h>
#include <string.h>
#include <numeric_constants.h>

static inline float sliding_function(sliding_mode_observer_t* const instance, float est_err)
{
	float retVal = 0.0f;

    if (est_err > instance->boundary)
    {
    	retVal = 1.0f;
    }
    else if (est_err < -instance->boundary)
	{
    	retVal = -1.0f;
    }
    else
    {
    	retVal = est_err * instance->inv_boundary;
    }
    return retVal;
}


void sliding_mode_observer_init(sliding_mode_observer_t* const instance, const sliding_mode_observer_cfg_t *cfg)
{
	assert(instance != NULL && cfg != NULL);

	float Rs = cfg->rs * 1e-3f;   // mΩ → Ω
	float Ls = cfg->ls * 1e-3f;   // mH → H

	instance->a1 = expf(-(Rs /Ls) * cfg->ts);
	instance->b1 = (1.0f - instance->a1) / Rs;
	instance->b1_inv = 1.0f / instance->b1;
	instance->k_sliding_gain = cfg->k_sliding_gain;
	instance->g_emf_gain = cfg->g_emf_gain;
	instance->boundary = cfg->boundary;
	instance->inv_boundary = (1.0f / instance->boundary);
	instance->ts = cfg->ts;
	instance->inv_ts = 1.0f / instance->ts;

	instance->i_alpha_est = 0.0f;
	instance->i_beta_est = 0.0f;
	instance->e_alpha_est = 0.0f;
	instance->e_beta_est = 0.0f;
	instance->emf_threshold = cfg->emf_threshold;
	instance->valid_emf_sample_cntr_threshold = cfg->emf_cntr_threshold;

	instance->theta_est = 0.0f;
	instance->theta_est_prev = 0.0f;
	instance->omega_est = 0.0f;

	instance->i_alpha_error = 0.0f;
	instance->i_beta_error = 0.0f;
	instance->emf_mag = 0.0f;

	memset(instance->delta_theta_buf, 0.0f, sizeof(instance->delta_theta_buf));
	instance->delta_theta_sum = 0.0f;
	instance->inv_n_ts = 1.0f / (SMO_MOVING_AVG_FILTER_LEN * instance->ts);
	instance->idx = 0;

}


void sliding_mode_observer_process(sliding_mode_observer_t* const instance, float v_alpha, float v_beta,  float i_alpha, float i_beta)
{
	assert(instance != NULL);

//========================================================================
	// 1. Calculate the alpha-, beta- current estimation error
	float i_alpha_est_err = instance->i_alpha_est - i_alpha;
	float i_beta_est_err = instance->i_beta_est  - i_beta;

//========================================================================
	// 2. Calculate and apply the sliding injection
	float z_alpha = instance->k_sliding_gain * sliding_function(instance, i_alpha_est_err);
	float z_beta = instance->k_sliding_gain * sliding_function(instance, i_beta_est_err);

//========================================================================
	// 3. ZOH-based, discrete-time current-observer
	instance->i_alpha_est = instance->a1 * instance->i_alpha_est + instance->b1 * (v_alpha - instance->e_alpha_est) - z_alpha;
	instance->i_beta_est = instance->a1 * instance->i_beta_est + instance->b1 * (v_beta - instance->e_beta_est) - z_beta;

//========================================================================
	// 4. Error-based, discrete-time BEMF-observer
	instance->e_alpha_est += (instance->g_emf_gain * instance->b1_inv * (i_alpha_est_err - instance->a1 * instance->i_alpha_error + z_alpha));
	instance->e_beta_est += (instance->g_emf_gain * instance->b1_inv * (i_beta_est_err - instance->a1 * instance->i_beta_error + z_beta));

//========================================================================
	// 5. Save the previous state
	instance->i_alpha_error = i_alpha_est_err;
	instance->i_beta_error  = i_beta_est_err;

//	// 6. Estimate the rotor's electrical angle and -speed
	instance->emf_mag = sqrtf(instance->e_alpha_est * instance->e_alpha_est + instance->e_beta_est * instance->e_beta_est);
	if (instance->emf_mag > instance->emf_threshold)
	{
		instance->valid_emf_sample_cntr++;
		if(instance->valid_emf_sample_cntr >= instance->valid_emf_sample_cntr_threshold)
		{
			instance->smo_locked = true;
		}

		instance->e_theta = atan2f(instance->e_beta_est, instance->e_alpha_est);
		if(instance->omega_est > 0.0f)
		{
			instance->theta_est = instance->e_theta - (CONSTANT_PI * ONE_BY_TWO);
		}
		else
		{
			instance->theta_est = instance->e_theta + (CONSTANT_PI * ONE_BY_TWO);
		}

		if(instance->theta_est < 0.0f)
		{
			instance->theta_est += CONSTANT_TWO_PI;
		}
		else if(instance->theta_est > CONSTANT_TWO_PI)
		{
			instance->theta_est -= CONSTANT_TWO_PI;
		}

		float delta_theta = instance->e_theta - instance->e_theta_last;

		if (delta_theta >= CONSTANT_PI)
		{
			delta_theta -= CONSTANT_TWO_PI;
		}
		else if (delta_theta <= -CONSTANT_PI)
		{
			delta_theta += CONSTANT_TWO_PI;
		}
		instance->delta_theta_sum -= instance->delta_theta_buf[instance->idx];
		instance->delta_theta_buf[instance->idx] = delta_theta;
		instance->delta_theta_sum += delta_theta;
		instance->idx++;
		if (instance->idx >= SMO_MOVING_AVG_FILTER_LEN)
		{
			instance->idx = 0;
		}
		instance->omega_est = instance->delta_theta_sum * instance->inv_n_ts;
		instance->e_theta_last = instance->e_theta;
	}
	else
	{
		instance->valid_emf_sample_cntr = 0u;
		instance->smo_locked = false;
	}
}

void sliding_mode_observer_reset(sliding_mode_observer_t* const instance)
{
	assert(instance != NULL);
	instance->i_alpha_est 		= 0.0f;
	instance->i_beta_est		= 0.0f;
	instance->e_alpha_est  		= 0.0f;
	instance->e_beta_est		= 0.0f;
	instance->i_alpha_error 	= 0.0f;
	instance->i_beta_error 		= 0.0f;
	instance->theta_est 		= 0.0f;
	instance->theta_est_prev	= 0.0f;
	instance->omega_est 		= 0.0f;
	instance->emf_mag 			= 0.0f;

}

sliding_mode_observer_emf_est_t sliding_mode_observer_get_emfs(sliding_mode_observer_t* const instance)
{
	sliding_mode_observer_emf_est_t retVal = {0};
	retVal.emf_alfa = instance->e_alpha_est;
	retVal.emf_beta = instance->e_beta_est;
	return retVal;
}

float sliding_mode_observer_get_electrical_angle(sliding_mode_observer_t* const instance)
{
	assert(instance != NULL);
	float ret_val = 0.0f;
	ret_val = instance->theta_est;
	return ret_val;
}

float sliding_mode_observer_get_electrical_speed(sliding_mode_observer_t* const instance)
{
	assert(instance != NULL);
	float ret_val = 0.0f;
	ret_val = instance->omega_est;
	return ret_val;
}

bool_t sliding_mode_observer_is_locked(sliding_mode_observer_t* const instance)
{
	assert(instance != NULL);
	bool_t ret_val = instance->smo_locked;
	return ret_val;
}



