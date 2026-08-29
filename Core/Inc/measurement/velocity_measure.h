#ifndef VELOCITY_MEASURE_H_
#define VELOCITY_MEASURE_H_

#include <lpf_first_order.h>

typedef struct velocity_measure_config velocity_measure_config_t;
typedef struct velocity_measure velocity_measure_t;

struct velocity_measure_config {
	uint32_t kv_value;
	uint32_t pole_pairs;
	lpf_first_order_t* lpf_fo_instance;

};

struct velocity_measure {
	uint32_t kv_value;
	uint32_t pole_pairs;
	lpf_first_order_t *lpf_fo_instance;
	float_t rpm;
};


void velocity_measure_init(velocity_measure_t *const instance, velocity_measure_config_t *cfg);

void velocity_measure_process(velocity_measure_t *const instance, float_t omega_electrical_rad_s);

float_t velocity_measure_get_rpm(velocity_measure_t *const instance);

#endif /* VELOCITY_MEASURE_H_ */
