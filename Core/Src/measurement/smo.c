#include <config/motor_cfg.h>
#include <smo.h>
#include <string.h>

static inline smo_sliding_funtion(float error, float boundary, float inv_linear_region_boundary)
{
    if (error >= boundary) return 1.0f;
    if (error <= -boundary) return -1.0f;
    return error * inv_linear_region_boundary;
}

void smo_init(smo_t* const instance, const smo_cfg_t *cfg)
{
	assert(instance != NULL && cfg != NULL);

	float Rs = cfg->R_stator * 1e-3f;   // mΩ → Ω
	float Ls = cfg->L_stator * 1e-3f;   // mH → H

	instance->A = expf(-(Rs /Ls) * cfg->ts);
	instance->B = (1.0f - instance->A) / Rs;
	instance->B_inv = 1.0f/instance->B;

	instance->eta_curr_obsv_gain = cfg->eta_curr_obsv_gain;
	instance->g_bemf_obsv_gain = cfg->g_emf_gain;

	instance->boundary = cfg->boundary;
	instance->inv_boundary = (1.0f / instance->boundary);
	instance->ts = cfg->ts;
	instance->inv_ts = 1.0f / instance->ts;
	instance->omega_lpf_gain = cfg->omega_lpf_gain;

	instance->i_alpha_est = 0.0f;
	instance->i_beta_est = 0.0f;
	instance->e_alpha_est = 0.0f;
	instance->e_beta_est = 0.0f;
	instance->theta_est = 0.0f;
	instance->theta_est_prev = 0.0f;
	instance->omega_est = 0.0f;
	instance->omega_filtered = 0.0f;

	instance->last_i_alpha_error = 0.0f;
	instance->last_i_beta_error = 0.0f;
	instance->emf_mag = 0.0f;

	memset(instance->delta_theta_buf, 0.0f, sizeof(instance->delta_theta_buf));
	instance->delta_theta_sum = 0.0f;
	instance->inv_n_ts = 1.0f / (SMO_MOVING_AVG_FILTER_LEN * instance->ts);
	instance->idx = 0;

}

void smo_process(smo_t* const instance, float v_alpha, float v_beta,  float i_alpha, float i_beta)
{
	float i_alpha_error = instance->i_alpha_est - i_alpha;
	float i_beta_error = instance->i_beta_est - i_beta;

	float z_alpha = instance->eta_curr_obsv_gain * smo_sliding_funtion(i_alpha_error, instance->boundary, instance->inv_boundary);
	float z_beta = instance->eta_curr_obsv_gain * smo_sliding_funtion(i_beta_error, instance->boundary, instance->inv_boundary);

	instance->i_alpha_est = instance->A * instance->i_alpha_est + instance->B * (v_alpha - instance->e_alpha_est) - z_alpha;
	instance->i_beta_est = instance->A * instance->i_beta_est + instance->B * (v_beta - instance->e_beta_est) - z_beta;

	float last_z_alpha = instance->eta_curr_obsv_gain * smo_sliding_funtion(instance->last_i_alpha_error, instance->boundary, instance->inv_boundary);
	float last_z_beta = instance->eta_curr_obsv_gain * smo_sliding_funtion(instance->last_i_beta_error, instance->boundary, instance->inv_boundary);

	instance->e_alpha_est += instance->B_inv * instance->g_bemf_obsv_gain * (i_alpha_error - instance->A * instance->last_i_alpha_error + last_z_alpha);
	instance->e_beta_est += instance->B_inv * instance->g_bemf_obsv_gain * (i_beta_error - instance->A * instance->last_i_beta_error + last_z_beta);

	instance->last_i_alpha_error = i_alpha_error;
	instance->last_i_beta_error = i_beta_error;

	instance->emf_mag = sqrtf(instance->e_alpha_est*instance->e_alpha_est + instance->e_beta_est * instance->e_beta_est);

	instance->theta_est = atan2f(instance->e_beta_est, instance->e_alpha_est);


	// Apply moving average
	float delta_theta = instance->theta_est - instance->theta_est_prev;
	if (delta_theta >= CONSTANT_PI)
	{
		delta_theta -= CONSTANT_TWO_PI;
	}
	else if (delta_theta <= -CONSTANT_PI)
	{
		delta_theta += CONSTANT_TWO_PI;
	}
	instance->theta_est_prev = instance->theta_est;
	instance->delta_theta_sum -= instance->delta_theta_buf[instance->idx];
	instance->delta_theta_buf[instance->idx] = delta_theta;
	instance->delta_theta_sum += delta_theta;
	instance->idx++;
	if (instance->idx >= SMO_MOVING_AVG_FILTER_LEN)
	{
		instance->idx = 0;
	}
	instance->omega_est = instance->delta_theta_sum * instance->inv_n_ts;

	// Apply low-pass filter
	instance->omega_filtered = (1.0f - instance->omega_lpf_gain * instance->ts)*instance->omega_filtered + instance->omega_lpf_gain * instance->ts*instance->omega_est;
}

void smo_reset(smo_t* const instance)
{
    assert(instance != NULL);

    instance->i_alpha_est = 0.0f;
    instance->i_beta_est = 0.0f;

    instance->e_alpha_est = 0.0f;
    instance->e_beta_est = 0.0f;

    instance->theta_est = 0.0f;
    instance->theta_est_prev = 0.0f;

    instance->omega_est = 0.0f;
    instance->omega_filtered = 0.0f;

    instance->last_i_alpha_error = 0.0f;
    instance->last_i_beta_error = 0.0f;

    instance->emf_mag = 0.0f;

    instance->delta_theta_sum = 0.0f;
    instance->idx = 0u;

    for (uint32_t i = 0; i < SMO_MOVING_AVG_FILTER_LEN; i++)
    {
        instance->delta_theta_buf[i] = 0.0f;
    }
}


smo_emf_est_t smo_get_emfs(smo_t* const instance)
{
	smo_emf_est_t ret_val = {0};
	ret_val.emf_alfa = instance->e_alpha_est;
	ret_val.emf_beta = instance->e_beta_est;
	return ret_val;

}

float smo_get_electrical_angle(smo_t* const instance)
{
	float ret_val = 0.0f;
	ret_val = instance->theta_est;
	return ret_val;

}

float smo_get_electrical_speed(smo_t* const instance)
{
	float ret_val = 0.0f;
	ret_val = instance->omega_filtered;
	return ret_val;
}
