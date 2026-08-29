#ifndef SMO_H_
#define SMO_H_


#define SMO_MOVING_AVG_FILTER_LEN	10u

typedef struct smo_cfg smo_cfg_t;

typedef struct smo smo_t;

typedef struct smo_emf_est smo_emf_est_t;


struct smo_cfg {
	float R_stator;
	float L_stator;
	float ts;
	float boundary;
	float eta_curr_obsv_gain;
	float g_emf_gain;
	float omega_lpf_gain;
};


struct smo {
	float R_stator;
	float L_stator;
	float A;
	float B;
	float B_inv;
	float eta_curr_obsv_gain;
	float g_bemf_obsv_gain;
	float ts;
	float inv_ts;
	float boundary;
	float inv_boundary;
	float omega_lpf_gain;

	float i_alpha_est;
	float i_beta_est;
	float last_i_alpha_error;
	float last_i_beta_error;
	float e_alpha_est;
	float e_beta_est;
	float theta_est;
	float theta_est_prev;
	float omega_est;
	float omega_filtered;
	float emf_mag;
	float delta_theta_buf[SMO_MOVING_AVG_FILTER_LEN];
	float delta_theta_sum;
	uint8_t idx;
	float inv_n_ts;
};

struct smo_emf_est
{
	float emf_alfa;
	float emf_beta;
};

void smo_init(smo_t* const instance, const smo_cfg_t *cfg);

void smo_process(smo_t* const instance, float v_alpha, float v_beta,  float i_alpha, float i_beta);

void smo_reset(smo_t* const instance);

smo_emf_est_t smo_get_emfs(smo_t* const instance);

float smo_get_electrical_angle(smo_t* const instance);

float smo_get_electrical_speed(smo_t* const instance);



#endif /* SMO_H_ */
