#ifndef SLIDING_MODE_OBSERVER_H_
#define SLIDING_MODE_OBSERVER_H_

#define SMO_MOVING_AVG_FILTER_LEN	128


typedef struct sliding_mode_observer_cfg sliding_mode_observer_cfg_t;

typedef struct sliding_mode_observer sliding_mode_observer_t;

typedef struct sliding_mode_observer_emf_est sliding_mode_observer_emf_est_t;

struct sliding_mode_observer_cfg {
	float rs;
	float ls;
	float ts;
	float k_sliding_gain;
	float g_emf_gain;
	float boundary;
	float emf_threshold;
	float emf_cntr_threshold;
};

struct sliding_mode_observer {
	float a1;
	float b1;
	float b1_inv;
	float k_sliding_gain;
	float g_emf_gain;
	float boundary;
	float inv_boundary;
	float ts;
	float inv_ts;

	float i_alpha_est;
	float i_beta_est;
	float e_alpha_est;
	float e_beta_est;
	float e_theta;
	float e_theta_last;
	float emf_threshold;
	uint32_t valid_emf_sample_cntr;
	uint32_t valid_emf_sample_cntr_threshold;
	bool_t smo_locked;

	float theta_est;
	float theta_est_prev;
	float delta_theta_buf[SMO_MOVING_AVG_FILTER_LEN];
	float delta_theta_sum;
	float omega_est;
	uint8_t idx;
	float inv_n_ts;

	float i_alpha_error;
	float i_beta_error;
	float emf_mag;
};

struct sliding_mode_observer_emf_est
{
	float emf_alfa;
	float emf_beta;
};


void sliding_mode_observer_init(sliding_mode_observer_t* const instance, const sliding_mode_observer_cfg_t *cfg);

void sliding_mode_observer_process(sliding_mode_observer_t* const instance, float v_alpha, float v_beta,  float i_alpha, float i_beta);

void sliding_mode_observer_reset(sliding_mode_observer_t* const instance);

sliding_mode_observer_emf_est_t sliding_mode_observer_get_emfs(sliding_mode_observer_t* const instance);

float sliding_mode_observer_get_electrical_angle(sliding_mode_observer_t* const instance);

float sliding_mode_observer_get_electrical_speed(sliding_mode_observer_t* const instance);

bool_t sliding_mode_observer_is_locked(sliding_mode_observer_t* const instance);
#endif /* SLIDING_MODE_OBSERVER_H_ */
