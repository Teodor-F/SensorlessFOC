#ifndef CURRENT_CONTROLLER_H
#define CURRENT_CONTROLLER_H

#include <pi_controller.h>

typedef struct current_controller_cfg current_controller_cfg_t;
typedef struct current_controller current_controller_t;
typedef struct current_controller_output current_controller_output_t;

struct current_controller_cfg {
	float kp_id;
	float ki_id;
	float kp_iq;
	float ki_iq;
	float ts;					// [s]
	float vd_out_limit;			// [mV]
	float vq_out_limit;			// [mV]
};

struct current_controller {
	pi_controller_t id_pi_cntrl;
	pi_controller_t iq_pi_cntrl;
	float ts;						// [s]
	float id_ref;					// [mA]
	float iq_ref;					// [mA]
	float vd_out;					// [mV]
	float vq_out;					// [mV]
};

struct current_controller_output {
	float vd;	// [mV]
	float vq;	// [mV]
};

void current_controller_init(current_controller_t* const instance, const current_controller_cfg_t *cfg);

void current_controller_process(current_controller_t* const instance, float id_error, float iq_error);

void current_controller_set_target_id(current_controller_t* const instance, float new_id);

void current_controller_set_target_iq(current_controller_t* const instance, float new_iq);

float current_controller_get_target_id(current_controller_t* const instance);

float current_controller_get_target_iq(current_controller_t* const instance);

current_controller_output_t current_controller_get_vd_vq_out(current_controller_t* const instance);

void current_controller_reset(current_controller_t* const instance);

#endif // CURRENT_CONTROLLER_H
