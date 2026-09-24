#ifndef SLIDING_MODE_OBSERVER_H_
#define SLIDING_MODE_OBSERVER_H_

#define SMO_MOVING_AVG_FILTER_LEN	128


typedef struct sliding_mode_observer_cfg sliding_mode_observer_cfg_t;

typedef struct sliding_mode_observer sliding_mode_observer_t;

typedef struct sliding_mode_observer_emf_est sliding_mode_observer_emf_est_t;

struct sliding_mode_observer_cfg {
	float_t rs;
	float_t ls;
	float_t k_sliding_gain;
	float_t g_emf_gain;
	float_t boundary;
	float_t emf_threshold;
	float_t emf_cntr_threshold;
	motor_control_cycle_time_t cycle_time;
};

struct sliding_mode_observer {
	float_t a1;
	float_t b1;
	float_t b1_inv;
	float_t k_sliding_gain;
	float_t g_emf_gain;
	float_t boundary;
	float_t inv_boundary;
	float_t sampling_time;
	float_t inv_ts;

	float_t i_alpha_est;
	float_t i_beta_est;
	float_t e_alpha_est;
	float_t e_beta_est;
	float_t e_theta;
	float_t e_theta_last;
	float_t emf_threshold;
	uint32_t valid_emf_sample_cntr;
	uint32_t valid_emf_sample_cntr_threshold;
	bool_t smo_locked;

	float_t theta_est;
	float_t theta_est_prev;
	float_t delta_theta_buf[SMO_MOVING_AVG_FILTER_LEN];
	float_t delta_theta_sum;
	float_t omega_est;
	uint8_t idx;
	float_t inv_n_ts;

	float_t i_alpha_error;
	float_t i_beta_error;
	float_t emf_mag;
};

struct sliding_mode_observer_emf_est
{
	float_t emf_alfa;
	float_t emf_beta;
};


void sliding_mode_observer_init(sliding_mode_observer_t* const instance, const sliding_mode_observer_cfg_t *cfg);

void sliding_mode_observer_process(sliding_mode_observer_t* const instance, float_t v_alpha, float_t v_beta,  float_t i_alpha, float_t i_beta);

void sliding_mode_observer_reset(sliding_mode_observer_t* const instance);

sliding_mode_observer_emf_est_t sliding_mode_observer_get_emfs(sliding_mode_observer_t* const instance);

float_t sliding_mode_observer_get_electrical_angle(sliding_mode_observer_t* const instance);

float_t sliding_mode_observer_get_electrical_speed(sliding_mode_observer_t* const instance);

bool_t sliding_mode_observer_is_locked(sliding_mode_observer_t* const instance);
#endif /* SLIDING_MODE_OBSERVER_H_ */
