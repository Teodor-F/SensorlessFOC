#ifndef PLL_H_
#define PLL_H_

typedef struct pll_cfg pll_cfg_t;
typedef struct pll pll_t;

struct pll_cfg	{
	float kp;
	float ki;
	float ts;
	float omega_max;
};

struct pll {
	float kp;
	float ki;
	float ts;
	float ki_ts;
	float omega_est;
	float omega_integral;
	float omega_max;
	float theta_est;
};

void pll_init(pll_t* const instance, const pll_cfg_t* cfg);

void pll_process(pll_t* const instance, float emf_alpha_est, float emf_beta_est);

void pll_process_new(pll_t* const instance, float emf_alpha_est, float emf_beta_est);

void pll_reset(pll_t* const instance);

float pll_get_est_theta(const pll_t* const instance);

float pll_get_est_omega(const pll_t* const instance);

#endif /* PLL_H_ */
