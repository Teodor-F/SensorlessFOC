#include <config/motor_cfg.h>
#include <numeric_constants.h>


void sv_clarke_transform(float curr_a, float curr_b, float curr_c, float *curr_alpha, float *curr_beta)
{
	*curr_alpha = curr_a;
	*curr_beta = (ONE_BY_SQRT_THREE) * (curr_b - curr_c);
}

void sv_park_transform(float curr_alpha, float curr_beta, float sin_theta, float cos_theta, float *curr_d, float *curr_q)
{
	*curr_d = curr_alpha * cos_theta + curr_beta * sin_theta;
	*curr_q = curr_beta * cos_theta - curr_alpha * sin_theta;
}

void sv_inv_park_transform(float volt_d, float volt_q, float sin_theta, float cos_theta, float *volt_alpha, float *volt_beta)
{
	*volt_alpha = volt_d * cos_theta - volt_q * sin_theta;
	*volt_beta = volt_d * sin_theta + volt_q * cos_theta;
}

void sv_inv_clarke_transform(float v_alfa, float v_beta, float *v_a, float *v_b, float *v_c)
{
	*v_a = v_alfa;
	*v_b = -ONE_BY_TWO * v_alfa + SQRT_THREE_BY_TWO * v_beta;
	*v_c = -ONE_BY_TWO * v_alfa - SQRT_THREE_BY_TWO * v_beta;
}

