#ifndef SV_TRANSFORMATIONS_H_
#define SV_TRANSFORMATIONS_H_


void sv_clarke_transform(float curr_a, float curr_b, float curr_c, float *curr_alpha, float *curr_beta);

void sv_park_transform(float curr_alpha, float curr_beta, float sin_theta, float cos_theta, float *curr_d, float *curr_q);

void sv_inv_park_transform(float volt_d, float volt_q, float sin_theta, float cos_theta, float *volt_alpha, float *volt_beta);

void sv_inv_clarke_transform(float v_alfa, float v_beta, float *v_a, float *v_b, float *v_c);


#endif /* SV_TRANSFORMATIONS_H_ */
