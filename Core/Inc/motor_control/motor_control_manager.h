#ifndef MOTOR_CONTROL_MANAGER_H_
#define MOTOR_CONTROL_MANAGER_H_

#include <stdbool.h>
#include <foc_monitor.h>

//========================================================================
	// Communication layer
extern volatile bool foc_telemetry_ready;
extern volatile foc_monitor_frame_t monitor_frame;


typedef enum motor_control_manager_direction motor_control_manager_direction_t;
typedef enum motor_control_manager_state motor_control_manager_state_t;
typedef struct motor_control_manager_cfg motor_control_manager_cfg_t;
typedef struct motor_control_manager motor_control_manager_t;

enum motor_control_manager_direction {
	MC_DIR_CW,
	MC_DIR_CCW
};

enum motor_control_manager_state {
	MC_STATE_IDLE,
	MC_STATE_ALIGN,
	MC_STATE_OPEN_LOOP,
	MC_STATE_TRANSITION,
	MC_STATE_CLOSED_LOOP,
	MC_STATE_ERROR
};

struct motor_control_manager_cfg {
	float_t pwm_freq;
	uint32_t alignment_time;
	float_t aligment_id;
	float_t aligment_iq;
	uint32_t open_loop_ramp_time;
	uint32_t open_loop_velocity_setpoint;
	float_t	open_loop_id;
	float_t open_loop_iq;
	float_t transition_iq;
	uint32_t transition_time;
};

struct motor_control_manager {
//========================================================================
	// Internal config
	motor_control_manager_cfg_t cfg;
	motor_control_manager_direction_t mc_direction;
	motor_control_manager_state_t mc_state;
	float_t sampling_time;
//========================================================================
	// Reference- and setpoint quantities
	float_t theta_ref_log;
	float_t theta_ref;						//[rad]
	float_t omega_ref;						//[rad/s]
	float_t velocity_setpoint;				//[rpm]
	float_t vd_setpoint;					//[mV]
	float_t vq_setpoint;					//[mV]
	float_t id_setpoint;					//[mA]
	float_t iq_setpoint;					//[ma]



//========================================================================
	// Rotor Alignment
	uint32_t aligment_tick_counter;
	uint32_t aligment_ticks;
//========================================================================
	// Open-loop startup
	uint32_t open_loop_ramp_time_tick_counter;
	uint32_t open_loop_ramp_time_ticks;
	float_t open_loop_omega_max;			//[rad/s]
	float_t open_loop_omega_dt;				//[rad/s2]
	float_t theta_open_loop;				//[rad]
	float_t omega_open_loop;				//[rad/s]
	float_t omega_coeff;					//[-1/1]
//========================================================================
	// Stabilization & transition
	uint32_t transition_time_tick_counter;
	uint32_t transition_time_ticks;
	float_t theta_offset_arr[64];
	uint32_t theta_offset_arr_idx;
	uint32_t theta_offset_arr_size;
	float_t theta_offset;
	bool_t theta_offset_calculated;
};

void motor_control_manager_init(motor_control_manager_cfg_t const *cfg);

#endif /* MOTOR_CONTROL_MANAGER_H_ */
