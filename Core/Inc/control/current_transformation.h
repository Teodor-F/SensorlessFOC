#ifndef CURRENT_TRANSFORMATION_H_
#define CURENT_TRANSFORMATION_H_

typedef struct current_transformation current_transformation_t;
typedef struct current_transformation_curr_t current_transformation_curr_t;

struct current_transformation {
	float_t current_alfa;
	float_t current_beta;
	float_t current_d;
	float_t current_q;
};

struct current_transformation_curr_t {
	float_t current_alfa;
	float_t current_beta;
	float_t current_d;
	float_t current_q;
};


void current_transformation_init(current_transformation_t *const instance);

void current_transformation_process(current_transformation_t *const instance, float_t current_a,  float_t current_b,  float_t current_c, float_t theta);

current_transformation_curr_t current_transformation_get_currents(current_transformation_t *const instance);

void current_transformation_reset(current_transformation_t *const instance);

#endif /* CURRENT_TRANSFORMATION_H_ */
