#include <sliding_mode_observer.h>
#include <assert.h>
#include <numeric_constants.h>

static inline float smo_sliding_function(smo_t* const instance, float est_err)
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


void smo_init(smo_t* const instance, const smo_cfg_t *cfg)
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

	instance->i_alpha_est = 0.0f;
	instance->i_beta_est = 0.0f;
	instance->e_alpha_est = 0.0f;
	instance->e_beta_est = 0.0f;
	instance->i_alpha_error = 0.0f;
	instance->i_beta_error = 0.0f;
}


void smo_process(smo_t* const instance, float v_alpha, float v_beta,  float i_alpha, float i_beta)
{
	assert(instance != NULL);

//========================================================================
	// 1. Calculate the alpha-, beta- current estimation error
	float i_alpha_est_err = instance->i_alpha_est - i_alpha;
	float i_beta_est_err = instance->i_beta_est  - i_beta;

//========================================================================
	// 2. Calculate and apply the sliding injection
	float z_alpha = instance->k_sliding_gain * smo_sliding_function(instance, i_alpha_est_err);
	float z_beta = instance->k_sliding_gain * smo_sliding_function(instance, i_beta_est_err);

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
}

void smo_reset(smo_t* const instance)
{
	assert(instance != NULL);
	instance->i_alpha_est 	= 0.0f;
	instance->i_beta_est	= 0.0f;
	instance->e_alpha_est  	= 0.0f;
	instance->e_beta_est	= 0.0f;
	instance->i_alpha_error = 0.0f;
	instance->i_beta_error 	= 0.0f;
}

smo_emf_est_t smo_get_est_emfs(smo_t* const instance)
{
	smo_emf_est_t retVal = {0};
	retVal.e_alpha = instance->e_alpha_est;
	retVal.e_beta = instance->e_beta_est;
	return retVal;
}

