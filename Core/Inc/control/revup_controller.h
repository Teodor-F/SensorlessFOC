#ifndef REVUP_CONTROLLER_H_
#define REVUP_CONTROLLER_H_

typedef enum revup_controller_state revup_controller_state_t;
typedef struct revup_controller_current_output revup_controller_current_output_t;
typedef struct revup_controller_config revup_controller_config_t;
typedef struct revup_controller revup_controller_t;

enum revup_controller_state {
	REVUP_CONTROLLER_IDLE,
	REVUP_CONTROLLER_ALIGMENT,
	REVUP_CONTROLLER_OPEN_LOOP_ACCELERATION,
	REVUP_CONTROLLER_STABILIZATION,
};

struct revup_controller_current_output {
	float_t id_ref;
	float_t iq_ref;
};

struct revup_controller_config {
	float_t pwm_freq;
	uint32_t alignment_time;
	float_t aligment_id;
	float_t aligment_iq;
	uint32_t open_loop_ramp_time;
	uint32_t open_loop_velocity_setpoint;
	float_t	open_loop_id;
	float_t open_loop_iq;
	float_t stabilization_id;
	float_t stabilization_iq;
	uint32_t stabilization_time;
};

struct revup_controller {
	revup_controller_config_t revup_cfg;
	revup_controller_state_t revup_cntrl_state;
	bool_t revup_finished;
	float_t ts;
	uint32_t aligment_tick_counter;
	uint32_t aligment_ticks;
	uint32_t open_loop_ramp_time_tick_counter;
	uint32_t open_loop_ramp_time_ticks;
	float_t open_loop_omega_max;				//[rad/s]
	float_t open_loop_omega;					//[rad/s]
	float_t open_loop_omega_dt;					//[rad/s2]
	float_t open_loop_theta;					//[rad]
	uint32_t stabilization_time_tick_counter;
	uint32_t stabilization_time_ticks;
	float_t theta_out;							//[rad]
	float_t id_out;								//[mA]
	float_t iq_out;								//[mA]

};

void revup_controller_init(revup_controller_t *const instance, const revup_controller_config_t *cfg);

void revup_controller_process(revup_controller_t *const instance);

void revup_controller_reset(revup_controller_t *const instance);

revup_controller_current_output_t revup_controller_get_output(revup_controller_t *const instance);

float_t revup_controller_get_open_loop_electrical_angle(revup_controller_t *const instance);

float_t revup_controller_get_stabilization_q_current(revup_controller_t *const instance);

uint32_t revup_controller_get_open_loop_velocity_setpoint(revup_controller_t *const instance);

bool_t revup_controller_activate(revup_controller_t *const instance);

bool_t revup_controller_is_finished(revup_controller_t *const instance);

#endif /* REVUP_CONTROLLER_H_ */
