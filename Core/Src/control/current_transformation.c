#include <current_transformation.h>

#include <sv_transformations.h>

void current_transformation_init(current_transformation_t *const instance)
{
	instance->current_alfa = 0.0f;
	instance->current_beta = 0.0f;
	instance->current_d = 0.0f;
	instance->current_q = 0.0f;
}

void current_transformation_process(current_transformation_t *const instance, float_t current_a,  float_t current_b,  float_t current_c, float_t theta)
{
	sv_clarke_transform(current_a, current_b, current_c, &(instance->current_alfa), &(instance->current_beta));
	sv_park_transform(instance->current_alfa, instance->current_beta, sinf(theta), cosf(theta), &(instance->current_d), &instance->current_q);
}

current_transformation_curr_t current_transformation_get_currents(current_transformation_t *const instance)
{
	current_transformation_curr_t ret_val = {0};
	ret_val.current_alfa = instance->current_alfa;
	ret_val.current_beta = instance->current_beta;
	ret_val.current_d = instance->current_d;
	ret_val.current_q = instance->current_q;
	return ret_val;
}
