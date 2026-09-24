#ifndef CURRENT_CONTROLLER_H
#define CURRENT_CONTROLLER_H

#include <pi_controller.h>

typedef struct current_controller_cfg current_controller_cfg_t;
typedef struct current_controller current_controller_t;
typedef struct current_controller_output current_controller_output_t;

struct current_controller_cfg {
	float_t kp_id;
	float_t ki_id;
	float_t kp_iq;
	float_t ki_iq;
	float_t vd_out_limit;						// [mV]
	float_t vq_out_limit;						// [mV]
	motor_control_cycle_time_t cycle_time;

};

struct current_controller {
	pi_controller_t id_pi_cntrl;
	pi_controller_t iq_pi_cntrl;
	float_t sampling_time;			// [s]
	float_t id_ref;					// [mA]
	float_t iq_ref;					// [mA]
	float_t vd_out;					// [mV]
	float_t vq_out;					// [mV]
};

struct current_controller_output {
	float_t vd;	// [mV]
	float_t vq;	// [mV]
};

void current_controller_init(current_controller_t* const instance, const current_controller_cfg_t *cfg);

void current_controller_process(current_controller_t* const instance, float_t id_error, float_t iq_error);

void current_controller_set_target_id(current_controller_t* const instance, float_t new_id);

void current_controller_set_target_iq(current_controller_t* const instance, float_t new_iq);

float_t current_controller_get_target_id(current_controller_t* const instance);

float_t current_controller_get_target_iq(current_controller_t* const instance);

current_controller_output_t current_controller_get_vd_vq_out(current_controller_t* const instance);

void current_controller_reset(current_controller_t* const instance);

#endif // CURRENT_CONTROLLER_H
