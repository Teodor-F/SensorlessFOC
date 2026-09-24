#ifndef SV_TRANSFORMATIONS_H_
#define SV_TRANSFORMATIONS_H_

#include <numeric_constants.h>

static inline void sv_clarke_transform(float_t curr_a, float_t curr_b, float_t curr_c, float_t *curr_alpha, float_t *curr_beta)
{
	*curr_alpha = curr_a;
	*curr_beta = (ONE_BY_SQRT_THREE) * (curr_b - curr_c);
}

static inline void sv_park_transform(float_t curr_alpha, float_t curr_beta, float_t sin_theta, float_t cos_theta, float_t *curr_d, float_t *curr_q)
{
	*curr_d = curr_alpha * cos_theta + curr_beta * sin_theta;
	*curr_q = curr_beta * cos_theta - curr_alpha * sin_theta;
}

static inline void sv_inv_park_transform(float_t volt_d, float_t volt_q, float_t sin_theta, float_t cos_theta, float_t *volt_alpha, float_t *volt_beta)
{
	*volt_alpha = volt_d * cos_theta - volt_q * sin_theta;
	*volt_beta = volt_d * sin_theta + volt_q * cos_theta;
}

static inline void sv_inv_clarke_transform(float_t v_alfa, float_t v_beta, float_t *v_a, float_t *v_b, float_t *v_c)
{
	*v_a = v_alfa;
	*v_b = -ONE_BY_TWO * v_alfa + SQRT_THREE_BY_TWO * v_beta;
	*v_c = -ONE_BY_TWO * v_alfa - SQRT_THREE_BY_TWO * v_beta;
}


#endif /* SV_TRANSFORMATIONS_H_ */
