#include <motor_control_manager.h>
#include <control_layer_initializer.h>
#include <measurement_layer_initializer.h>
#include <periph_layer_initializer.h>

#include <sv_transformations.h>
#include <motor_cfg.h>
#include <foc_monitor.h>

#define ONE_SECOND_IN_NANOSECONDS ((uint32_t)(1000000000u))

static motor_control_manager_t mc_mngr_instance = {0};

static inline void motor_control_manager_activate_request(motor_control_request_t mc_request)
{
	 mc_mngr_instance.motor_control_request[mc_request] = TRUE;
}

static inline void motor_control_manager_deactivate_request(motor_control_request_t mc_request)
{
	 mc_mngr_instance.motor_control_request[mc_request] = FALSE;
}

static inline bool_t motor_control_manager_is_request_active(motor_control_request_t mc_request)
{
	bool_t ret_val = mc_mngr_instance.motor_control_request[mc_request];
	return ret_val;
}

static float_t motor_control_manager_get_electrical_angle(void)
{
	float_t ret_val = 0.0f;
	if(mc_mngr_instance.mc_state == MC_STATE_REVUP)
	{
		ret_val = revup_controller_get_open_loop_electrical_angle(revup_controller);
	}
	else if(mc_mngr_instance.mc_state == MC_STATE_CLOSED_LOOP)
	{
		ret_val = sliding_mode_observer_get_electrical_angle(smo);
	}
	return ret_val;
}

static void motor_control_manager_reset(void)
{
	sv_modulation_reset(sv_modulation);
	current_controller_reset(current_controller);
	velocity_controller_reset(velocity_controller);
	current_transformation_reset(current_transformation);
	revup_controller_reset(revup_controller);

	sliding_mode_observer_reset(smo);
	velocity_measure_reset(velocity_measure);
}

static void motor_control_manager_register_subtask(motor_control_subtask_t *const subtask, motor_control_cycle_time_t cycle_time, mc_callback_function_t subtask_function, mc_callback_param_t subtask_param)
{
	subtask->cycle_time = cycle_time;
	subtask->cycle_cnt = 0u;
	subtask->cycle_cnt_max = (uint32_t)cycle_time / mc_mngr_instance.time_base;
	assert(subtask->cycle_cnt_max > 0u);
	subtask->subtask_func = subtask_function;
	subtask->subtask_param = subtask_param;
}

static void motor_control_manager_execute_subtask(motor_control_subtask_t *const subtask)
{
	subtask->cycle_cnt++;
	if(subtask->cycle_cnt >= subtask->cycle_cnt_max)
	{
		if (subtask->subtask_func != NULL)
		{
			subtask->subtask_func(subtask->subtask_param);
		}
		subtask->cycle_cnt = 0;
	}
}

static void motor_control_state_machine(void)
{

	switch (mc_mngr_instance.mc_state)
	{
		case MC_STATE_IDLE:
		{
			if(motor_control_manager_is_request_active(MC_REVUP_REQUEST))
			{
				motor_control_manager_deactivate_request(MC_REVUP_REQUEST);
				revup_controller_activate(revup_controller);
				mc_mngr_instance.mc_state = MC_STATE_REVUP;
			}
			if(motor_control_manager_is_request_active(MC_RESET_REQUEST))
			{
				motor_control_manager_reset();
				motor_control_manager_deactivate_request(MC_RESET_REQUEST);
			}
			break;
		}
		case MC_STATE_REVUP:
		{
			if(motor_control_manager_is_request_active(MC_CLOSED_LOOP_REQUEST))
			{
				uint32_t revup_open_loop_velocity_max = revup_controller_get_open_loop_velocity_setpoint(revup_controller);
				float_t revup_stabilization_q_current = revup_controller_get_stabilization_q_current(revup_controller);
				velocity_controller_set_target_velocity(velocity_controller, revup_open_loop_velocity_max);
				velocity_controller_set_initial_value(velocity_controller, revup_stabilization_q_current);
				motor_control_manager_deactivate_request(MC_CLOSED_LOOP_REQUEST);
				mc_mngr_instance.mc_state = MC_STATE_CLOSED_LOOP;
			}
			break;
		}
		case MC_STATE_CLOSED_LOOP:
		{
			if(motor_control_manager_is_request_active(MC_STOP_REQUEST))
			{
				uint32_t revup_open_loop_velocity_max = revup_controller_get_open_loop_velocity_setpoint(revup_controller);
				velocity_controller_set_target_velocity(velocity_controller, revup_open_loop_velocity_max);
				if(velocity_measure_get_rpm(velocity_measure) <= revup_open_loop_velocity_max)
				{
					sv_modulation_set_target_vd_vq(sv_modulation, 0.0f,  0.0f);
					current_controller_set_target_id(current_controller, 0.0f);
					current_controller_set_target_iq(current_controller, 0.0f);
					velocity_controller_set_target_velocity(velocity_controller, 0u);
					motor_control_manager_deactivate_request(MC_STOP_REQUEST);
					motor_control_manager_activate_request(MC_RESET_REQUEST);
					mc_mngr_instance.mc_state = MC_STATE_IDLE;

				}
			}
			break;
		}
		case MC_STATE_ERROR:
		{
			break;
		}
	}
}


static void communication_subtask(mc_callback_param_t param)
{
	UNUSED(param);
	current_measure_act_curr_t curr_abc = current_measure_get_currents(current_measure);
	current_transformation_curr_t curr_clarke_park = current_transformation_get_currents(current_transformation);
	lpf_first_order_process(lpf_i_alfa, curr_clarke_park.current_alfa);
	lpf_first_order_process(lpf_i_beta, curr_clarke_park.current_beta);
	lpf_first_order_process(lpf_id, curr_clarke_park.current_d);
	lpf_first_order_process(lpf_iq,  curr_clarke_park.current_q);
	sliding_mode_observer_emf_est_t emf_alpha_beta = sliding_mode_observer_get_emfs(smo);
	float_t theta_smo = sliding_mode_observer_get_electrical_angle(smo);

	static foc_monitor_frame_t monitor_frame = {0};
	monitor_frame.header = FOC_FRAME_HEADER;
	monitor_frame.ia_mA = curr_abc.curr_a;
	monitor_frame.ib_mA = curr_abc.curr_b;
	monitor_frame.ic_mA = curr_abc.curr_c;
	monitor_frame.i_alfa_ma = lpf_first_order_get_filtered_value(lpf_i_alfa);
	monitor_frame.i_beta_ma = lpf_first_order_get_filtered_value(lpf_i_beta);
	monitor_frame.id_mA = lpf_first_order_get_filtered_value(lpf_id);
	monitor_frame.iq_mA = lpf_first_order_get_filtered_value(lpf_iq);
	monitor_frame.emf_alpha = emf_alpha_beta.emf_alfa;
	monitor_frame.emf_beta = emf_alpha_beta.emf_beta;
	monitor_frame.theta_observer_rad = theta_smo;
	monitor_frame.theta_ref_rad = motor_control_manager_get_electrical_angle();
	monitor_frame.velocity_observer_rpm = velocity_measure_get_rpm(velocity_measure);
	monitor_frame.velocity_ref_rpm = velocity_controller_get_target_velocity(velocity_controller);

	uart_transmit(uart, (const uint8_t*)&monitor_frame, sizeof(foc_monitor_frame_t));
}


static void velocity_measure_substask(mc_callback_param_t param)
{
	UNUSED(param);
	float_t observer_electrical_speed = sliding_mode_observer_get_electrical_speed(smo);
	velocity_measure_process(velocity_measure, observer_electrical_speed);
}


static void revup_controller_subtask(mc_callback_param_t param)
{
    UNUSED(param);

    if(mc_mngr_instance.mc_state == MC_STATE_REVUP)
    {
        revup_controller_process(revup_controller);
        revup_controller_current_output_t output = revup_controller_get_output(revup_controller);
        current_controller_set_target_id(current_controller, output.id_ref);
        current_controller_set_target_iq(current_controller, output.iq_ref);
        if(revup_controller_is_finished(revup_controller))
        {
        	motor_control_manager_activate_request(MC_CLOSED_LOOP_REQUEST);
        }
    }
}

static void current_transformation_subtask(mc_callback_param_t param)
{
	UNUSED(param);
	current_measure_process(current_measure);
	current_measure_act_curr_t curr_abc = current_measure_get_currents(current_measure);
	float_t electrical_angle = motor_control_manager_get_electrical_angle();
	current_transformation_process(current_transformation, curr_abc.curr_a,  curr_abc.curr_b,  curr_abc.curr_c, electrical_angle);
}

static void observer_subtask(mc_callback_param_t param)
{
	UNUSED(param);
	float_t v_alfa;
	float_t v_beta;
	sv_modulation_get_v_alfa_v_beta(sv_modulation, &v_alfa, &v_beta);
	current_transformation_curr_t curr_clarke_park = current_transformation_get_currents(current_transformation);
	sliding_mode_observer_process(smo, v_alfa, v_beta, curr_clarke_park.current_alfa, curr_clarke_park.current_beta);
}

static void velocity_control_subtask(mc_callback_param_t param)
{
	UNUSED(param);

	float_t rpm = velocity_measure_get_rpm(velocity_measure);
	if (mc_mngr_instance.mc_state == MC_STATE_CLOSED_LOOP)
	{
		velocity_controller_process(velocity_controller, rpm);
		float_t id = 0.0f;
		float_t iq = velocity_controller_get_current_out(velocity_controller);
		current_controller_set_target_id(current_controller, id);
		current_controller_set_target_iq(current_controller, iq);
	}
}


static void state_machine_subtask(mc_callback_param_t param)
{
	UNUSED(param);
	motor_control_state_machine();
}

static void current_controller_subtask(mc_callback_param_t param)
{
	UNUSED(param);
	if (mc_mngr_instance.mc_state != MC_STATE_IDLE && mc_mngr_instance.mc_state != MC_STATE_ERROR)
	{
		current_transformation_curr_t curr_clarke_park = current_transformation_get_currents(current_transformation);
		current_controller_process(current_controller, curr_clarke_park.current_d, curr_clarke_park.current_q);
		current_controller_output_t curr_cntrl_output = current_controller_get_vd_vq_out(current_controller);
		sv_modulation_set_target_vd_vq(sv_modulation, curr_cntrl_output.vd, curr_cntrl_output.vq);
	}
}

static void modulation_subtask(mc_callback_param_t param)
{
	UNUSED(param);
	float_t electrical_angle = motor_control_manager_get_electrical_angle();
	sv_modulation_process(sv_modulation, electrical_angle);
}

static void motor_control_task_1(mc_callback_param_t mc_task_1_param)
{
	UNUSED(mc_task_1_param);
	for (uint32_t sub_task_idx = 0; sub_task_idx < MC_TASK_1_COUNT; ++sub_task_idx)
	{
		motor_control_manager_execute_subtask(&(mc_mngr_instance.task_1_subtasks[sub_task_idx]));
	}
}

static void motor_control_task_2(mc_callback_param_t mc_task_2_param)
{
	UNUSED(mc_task_2_param);
	for (uint32_t sub_task_idx = 0; sub_task_idx < MC_TASK_2_COUNT; ++sub_task_idx)
	{
		motor_control_manager_execute_subtask(&(mc_mngr_instance.task_2_subtasks[sub_task_idx]));
	}
}


void motor_control_manager_init()
{
	mc_mngr_instance.mc_state = MC_STATE_IDLE;
	mc_mngr_instance.time_base = ONE_SECOND_IN_NANOSECONDS / mc_timer_get_freq(mc_timer);

	motor_control_manager_register_subtask(&mc_mngr_instance.task_1_subtasks[MC_TASK_1_COMMUNICATION], MOTOR_CONTROL_CYCLE_TIME_10000, communication_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_1_subtasks[MC_TASK_1_STATE_MACHINE], MOTOR_CONTROL_CYCLE_TIME_62_50, state_machine_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_CURRENT_TRANSFORMATION], MOTOR_CONTROL_CYCLE_TIME_62_50, current_transformation_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_REVUP], MOTOR_CONTROL_CYCLE_TIME_62_50, revup_controller_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_OBSERVER], MOTOR_CONTROL_CYCLE_TIME_62_50, observer_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_VELOCITY_MEASURE], MOTOR_CONTROL_CYCLE_TIME_62_50, velocity_measure_substask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_VELOCITY_CONTROL], MOTOR_CONTROL_CYCLE_TIME_250, velocity_control_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_CURRENT_CONTROL], MOTOR_CONTROL_CYCLE_TIME_62_50, current_controller_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_MODULATION], MOTOR_CONTROL_CYCLE_TIME_62_50, modulation_subtask, NULL);

	for (uint32_t idx = 0; idx < MC_REQUEST_COUNT; ++idx)
	{
		mc_mngr_instance.motor_control_request[idx] = FALSE;
	}

	mc_timer_register_mc_callback(mc_timer, MCTIMER_CB_IDX_1, motor_control_task_1, NULL);
	mc_timer_register_mc_callback(mc_timer, MCTIMER_CB_IDX_2, motor_control_task_2, NULL);
	mc_timer_activate_callback(mc_timer, MCTIMER_CB_IDX_1);
	mc_timer_activate_callback(mc_timer, MCTIMER_CB_IDX_2);

	// Synchronize timers
	mc_timer_start(mc_timer);
	pwm_start(pwm);
}


void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_13)
	{
		if(mc_mngr_instance.mc_state != MC_STATE_IDLE)
		{
			motor_control_manager_activate_request(MC_STOP_REQUEST);
		}
		else if(mc_mngr_instance.mc_state == MC_STATE_IDLE)
		{
			motor_control_manager_activate_request(MC_REVUP_REQUEST);
		}
	}
}

