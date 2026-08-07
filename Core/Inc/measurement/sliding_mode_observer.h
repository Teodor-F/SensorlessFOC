#ifndef SLIDING_MODE_OBSERVER_H_
#define SLIDING_MODE_OBSERVER_H_

typedef struct smo_cfg smo_cfg_t;

typedef struct smo smo_t;

typedef struct smo_emf_est smo_emf_est_t;

struct smo_cfg {
	float rs;
	float ls;
	float ts;
	float k_sliding_gain;
	float g_emf_gain;
	float boundary;
};

struct smo {
	float a1;
	float b1;
	float b1_inv;
	float k_sliding_gain;
	float g_emf_gain;
	float boundary;
	float inv_boundary;

	float i_alpha_est;
	float i_beta_est;
	float e_alpha_est;
	float e_beta_est;

	float i_alpha_error;
	float i_beta_error;
};

struct smo_emf_est
{
	float e_alpha;
	float e_beta;
};

void smo_init(smo_t* const instance, const smo_cfg_t *cfg);

void smo_process(smo_t* const instance, float v_alpha, float v_beta,  float i_alpha, float i_beta);

void smo_reset(smo_t* const instance);

smo_emf_est_t smo_get_est_emfs(smo_t* const instance);

#endif /* SLIDING_MODE_OBSERVER_H_ */
