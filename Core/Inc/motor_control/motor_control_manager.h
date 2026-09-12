#ifndef MOTOR_CONTROL_MANAGER_H_
#define MOTOR_CONTROL_MANAGER_H_

#include <mc_types.h>
#include <foc_monitor.h>
#include <mc_callback.h>

typedef enum motor_control_manager_direction motor_control_manager_direction_t;
typedef enum mc_task_1_subtask_type mc_task_1_subtask_type_t;
typedef enum mc_task_2_subtask_type mc_task_2_subtask_type_t;
typedef enum motor_control_manager_cycle_time motor_control_manager_cycle_time_t;
typedef enum motor_control_manager_state motor_control_manager_state_t;

typedef struct motor_control_subtask motor_control_subtask_t;
typedef struct motor_control_manager_cfg motor_control_manager_cfg_t;
typedef struct motor_control_manager motor_control_manager_t;

enum motor_control_manager_direction {
	MC_DIR_CW,
	MC_DIR_CCW
};

enum motor_control_manager_cycle_time {
    MOTION_CONTROL_MANAGER_CYCLE_TIME_62_50 = 62500u,       // 62.50 us
    MOTION_CONTROL_MANAGER_CYCLE_TIME_125   = 125000u,      // 125 us
	MOTION_CONTROL_MANAGER_CYCLE_TIME_250   = 250000u,      // 250 us
	MOTION_CONTROL_MANAGER_CYCLE_TIME_500   = 500000u,      // 500 us
	MOTION_CONTROL_MANAGER_CYCLE_TIME_1000  = 1000000u,     // 1000 us
	MOTION_CONTROL_MANAGER_CYCLE_TIME_2000  = 2000000u,		// 2000 us
	MOTION_CONTROL_MANAGER_CYCLE_TIME_5000	= 5000000u,		// 5000 us
	MOTION_CONTROL_MANAGER_CYCLE_TIME_10000	= 10000000u		// 10000 us
};

enum motor_control_manager_state {
	MC_STATE_IDLE,
	MC_STATE_ALIGN,
	MC_STATE_OPEN_LOOP,
	MC_STATE_TRANSITION,
	MC_STATE_CLOSED_LOOP,
	MC_STATE_ERROR
};

enum mc_task_1_subtask_type
{
    MC_TASK_1_COMMUNICATION = 0u,
    MC_TASK_1_STATE_MACHINE,
    MC_TASK_1_COUNT
};

enum mc_task_2_subtask_type
{
    MC_TASK_2_CURRENT_TRANSFORMATION = 0u,
    MC_TASK_2_OBSERVER,
    MC_TASK_2_VELOCITY_MEASURE,
    MC_TASK_2_VELOCITY_CONTROL,
    MC_TASK_2_CURRENT_CONTROL,
    MC_TASK_2_MODULATION,
    MC_TASK_2_COUNT
};

struct motor_control_subtask {
	motor_control_manager_cycle_time_t cycle_time;
	uint32_t cycle_cnt;
	uint32_t cycle_cnt_max;
	mc_callback_param_t subtask_param;
	mc_callback_function_t subtask_func;
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
	uint32_t time_base;
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

	motor_control_subtask_t task_1_subtasks[MC_TASK_1_COUNT];
	motor_control_subtask_t task_2_subtasks[MC_TASK_2_COUNT];
};

void motor_control_manager_init(motor_control_manager_cfg_t const *cfg);

#endif /* MOTOR_CONTROL_MANAGER_H_ */
