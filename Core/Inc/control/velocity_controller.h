#ifndef VELOCITY_CONTROLLER_H_
#define VELOCITY_CONTROLLER_H_

#include <pi_controller.h>

typedef struct velocity_controller_cfg velocity_controller_cfg_t;
typedef struct velocity_controller velocity_controller_t;

struct velocity_controller_cfg
{
	float_t kp;
	float_t ki;
	float_t ts;					// [s]
	float_t iq_max_out;			// [mA]
	float_t velocity_ref_limit;	// [rpm]
};

struct velocity_controller {
	pi_controller_t velocity_pi_cntrl;
	float_t ts;							// [s]
	float_t velocity_ref;				// [rpm]
	float_t iq_out;						// [mA]
};

void velocity_controller_init(velocity_controller_t *const instance, const velocity_controller_cfg_t *cfg);

void velocity_controller_process(velocity_controller_t *const instance, float_t velocity_error);

void velocity_controller_set_target_velocity(velocity_controller_t *const instance, float_t new_velocity);

float_t velocity_controller_get_target_velocity(velocity_controller_t *const instance);

float_t velocity_controller_get_current_out(velocity_controller_t *const instance);

void velocity_controller_set_initial_value(velocity_controller_t *const instance, float_t initial_value);

void velocity_controller_reset(velocity_controller_t *const instance);

#endif /* VELOCITY_CONTROLLER_H_ */
