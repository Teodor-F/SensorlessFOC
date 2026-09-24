#ifndef PLL_H_
#define PLL_H_

typedef struct pll_cfg pll_cfg_t;
typedef struct pll pll_t;

struct pll_cfg	{
	float_t kp;
	float_t ki;
	float_t ts;
	float_t omega_max;
};

struct pll {
	float_t kp;
	float_t ki;
	float_t ts;
	float_t ki_ts;
	float_t omega_est;
	float_t omega_integral;
	float_t omega_max;
	float_t theta_est;
};

void pll_init(pll_t* const instance, const pll_cfg_t* cfg);

void pll_process(pll_t* const instance, float_t emf_alpha_est, float_t emf_beta_est);

void pll_process_new(pll_t* const instance, float_t emf_alpha_est, float_t emf_beta_est);

void pll_reset(pll_t* const instance);

float_t pll_get_est_theta(const pll_t* const instance);

float_t pll_get_est_omega(const pll_t* const instance);

#endif /* PLL_H_ */
