#ifndef MOTOR_CONTROL_MANAGER_H_
#define MOTOR_CONTROL_MANAGER_H_

#include <mc_types.h>
#include <mc_callback.h>

#define CYCLE_TIME_DIVIDER ((uint32_t)(1000000000u))  // 1 second in nanoseconds


typedef enum motor_control_task_1_subtask_type motor_control_task_1_subtask_type_t;
typedef enum motor_control_task_2_subtask_type motor_control_task_2_subtask_type_t;
typedef enum motor_control_request motor_control_request_t;
typedef enum motor_control_cycle_time motor_control_cycle_time_t;
typedef enum motor_control_state motor_control_state_t;

typedef struct motor_control_subtask motor_control_subtask_t;
typedef struct motor_control_manager_cfg motor_control_manager_cfg_t;
typedef struct motor_control_manager motor_control_manager_t;


enum motor_control_cycle_time {
    MOTOR_CONTROL_CYCLE_TIME_62_50 	= 62500u,       // [us]
    MOTOR_CONTROL_CYCLE_TIME_125  	= 125000u,      // [us]
	MOTOR_CONTROL_CYCLE_TIME_250   	= 250000u,      // [us]
	MOTOR_CONTROL_CYCLE_TIME_500   	= 500000u,      // [us]
	MOTOR_CONTROL_CYCLE_TIME_1000  	= 1000000u,     // [us]
	MOTOR_CONTROL_CYCLE_TIME_2000  	= 2000000u,		// [us]
	MOTOR_CONTROLCYCLE_TIME_5000	= 5000000u,		// [us]
	MOTOR_CONTROL_CYCLE_TIME_10000	= 10000000u		// [us]
};

enum motor_control_state {
	MC_STATE_IDLE,
	MC_STATE_REVUP,
	MC_STATE_CLOSED_LOOP,
	MC_STATE_ERROR
};

enum motor_control_task_1_subtask_type
{
    MC_TASK_1_COMMUNICATION = 0u,
    MC_TASK_1_STATE_MACHINE,
    MC_TASK_1_VELOCITY_MEASURE,
    MC_TASK_1_VELOCITY_CONTROL,
    MC_TASK_1_COUNT
};

enum motor_control_task_2_subtask_type
{
    MC_TASK_2_CURRENT_TRANSFORMATION = 0u,
	MC_TASK_2_REVUP,
    MC_TASK_2_OBSERVER,
    MC_TASK_2_CURRENT_CONTROL,
    MC_TASK_2_MODULATION,
    MC_TASK_2_COUNT
};

enum motor_control_request
{
	MC_REVUP_REQUEST = 0u,
	MC_CLOSED_LOOP_REQUEST,
	MC_STOP_REQUEST,
	MC_RESET_REQUEST,
	MC_REQUEST_COUNT
};

struct motor_control_subtask {
	motor_control_cycle_time_t cycle_time;
	uint32_t cycle_cnt;
	uint32_t cycle_cnt_max;
	mc_callback_param_t subtask_param;
	mc_callback_function_t subtask_func;
};

struct motor_control_manager {
	motor_control_state_t mc_state;
	uint32_t time_base;
	motor_control_subtask_t task_1_subtasks[MC_TASK_1_COUNT];
	motor_control_subtask_t task_2_subtasks[MC_TASK_2_COUNT];
	bool_t motor_control_request[MC_REQUEST_COUNT];
};

void motor_control_manager_init();

#endif /* MOTOR_CONTROL_MANAGER_H_ */
