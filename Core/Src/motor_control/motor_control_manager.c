#include <motor_control_manager.h>
#include <control_layer_initializer.h>
#include <measurement_layer_initializer.h>
#include <periph_layer_initializer.h>

#include <sv_transformations.h>
#include <motor_cfg.h>
#include <foc_monitor.h>

#define ONE_SECOND_IN_NANOSECONDS ((uint32_t)(1000000000u))

static motor_control_manager_t mc_mngr_instance = {0};

static float_t get_omega_coeff(motor_control_manager_direction_t dir)
{
	float_t ret_val;
	if(dir == MC_DIR_CW)
	{
		ret_val = -1.0f;
	}
	else if(dir == MC_DIR_CCW)
	{
		ret_val = 1.0f;
	}
	return ret_val;
}

static inline bool_t ready_for_transition(void)
{
	bool_t ret_val = FALSE;
	if (mc_mngr_instance.omega_open_loop >= mc_mngr_instance.open_loop_omega_max)
	{
		ret_val = TRUE;
	}
	return ret_val;
}

static inline float_t constrain_angle(float_t angle)
{
	float_t ret_val = angle;
	if(fabsf(angle) >= CONSTANT_TWO_PI)
	{
		ret_val = 0.0f;
	}
	return ret_val;
}

static void motor_control_manager_register_subtask(motor_control_subtask_t *const subtask, motor_control_manager_cycle_time_t cycle_time , mc_callback_function_t subtask_function, mc_callback_param_t subtask_param)
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
			current_controller_reset(current_controller);
			mc_mngr_instance.vd_setpoint = 0.0f;
			mc_mngr_instance.vq_setpoint = 0.0f;
			break;
		}
//========================================================================
		// Align the motor to a predefined angle
		case MC_STATE_ALIGN:
		{
			mc_mngr_instance.theta_ref = 0.0f;
			mc_mngr_instance.theta_ref_log = 0.0f;
			current_controller_set_target_id(current_controller, mc_mngr_instance.cfg.aligment_iq);
			current_controller_set_target_iq(current_controller, mc_mngr_instance.cfg.aligment_iq);
			mc_mngr_instance.aligment_tick_counter++;
			if(mc_mngr_instance.aligment_tick_counter >= mc_mngr_instance.aligment_ticks)
			{
				mc_mngr_instance.aligment_tick_counter = 0u;
				mc_mngr_instance.theta_open_loop = 0.0f;
				mc_mngr_instance.omega_open_loop = 0.0f;
				current_controller_set_target_id(current_controller,  mc_mngr_instance.cfg.open_loop_id);
				current_controller_set_target_iq(current_controller, mc_mngr_instance.cfg.open_loop_iq);
				mc_mngr_instance.mc_state = MC_STATE_OPEN_LOOP;
			}
			break;
		}
//========================================================================
		// Spin the motor in speed open-loop  and current closed loop.
		case MC_STATE_OPEN_LOOP:
		{
			mc_mngr_instance.omega_open_loop += mc_mngr_instance.open_loop_omega_dt * mc_mngr_instance.sampling_time;
			if(ready_for_transition())
			{
				mc_mngr_instance.omega_open_loop = mc_mngr_instance.open_loop_omega_max;
				current_controller_set_target_iq(current_controller, mc_mngr_instance.cfg.transition_iq);
				mc_mngr_instance.transition_time_tick_counter = 0u;
				mc_mngr_instance.mc_state = MC_STATE_TRANSITION;
			}

			mc_mngr_instance.theta_open_loop += mc_mngr_instance.omega_coeff * mc_mngr_instance.omega_open_loop * mc_mngr_instance.sampling_time;
			mc_mngr_instance.theta_open_loop = constrain_angle(mc_mngr_instance.theta_open_loop);
			mc_mngr_instance.theta_ref = mc_mngr_instance.theta_open_loop;
			mc_mngr_instance.theta_ref_log = mc_mngr_instance.theta_ref;

			break;
		}
//========================================================================
		// Let the observer stabilize for a predefined time
		case MC_STATE_TRANSITION:
		{
			mc_mngr_instance.theta_open_loop += mc_mngr_instance.omega_coeff * mc_mngr_instance.omega_open_loop * mc_mngr_instance.sampling_time;
			mc_mngr_instance.theta_open_loop = constrain_angle(mc_mngr_instance.theta_open_loop);
			mc_mngr_instance.theta_ref = mc_mngr_instance.theta_open_loop;
			mc_mngr_instance.theta_ref_log = mc_mngr_instance.theta_ref;

			mc_mngr_instance.transition_time_tick_counter++;
			if (mc_mngr_instance.transition_time_tick_counter >= mc_mngr_instance.transition_time_ticks)
			{
				float_t transition_velocity = velocity_measure_get_rpm(velocity_measure);
				velocity_controller_set_target_velocity(velocity_controller, transition_velocity);
				velocity_controller_set_initial_value(velocity_controller, mc_mngr_instance.cfg.transition_iq);
				mc_mngr_instance.transition_time_tick_counter = 0u;
				mc_mngr_instance.mc_state = MC_STATE_CLOSED_LOOP;
			}
			break;
		}
		case MC_STATE_CLOSED_LOOP:
		{
			mc_mngr_instance.theta_ref = sliding_mode_observer_get_electrical_angle(smo);
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
	lpf_first_order_process(lpf_id, curr_clarke_park.current_d);
	lpf_first_order_process(lpf_iq,  curr_clarke_park.current_q);
	sliding_mode_observer_emf_est_t emf_alpha_beta = sliding_mode_observer_get_emfs(smo);
	float_t theta_smo = sliding_mode_observer_get_electrical_angle(smo);

	static foc_monitor_frame_t monitor_frame = {0};
	monitor_frame.header = FOC_FRAME_HEADER;
	monitor_frame.ia_mA = curr_abc.curr_a;
	monitor_frame.ib_mA = curr_abc.curr_b;
	monitor_frame.ic_mA = curr_abc.curr_c;
	monitor_frame.id_mA = lpf_first_order_get_filtered_value(lpf_id);
	monitor_frame.iq_mA = lpf_first_order_get_filtered_value(lpf_iq);
	monitor_frame.emf_alpha = emf_alpha_beta.emf_alfa;
	monitor_frame.emf_beta = emf_alpha_beta.emf_beta;
	monitor_frame.theta_real_rad = mc_mngr_instance.theta_open_loop;
	monitor_frame.theta_observer_rad = theta_smo;
	monitor_frame.theta_ref_log_rad = 0.0f;
	monitor_frame.velocity_pll_rpm = velocity_measure_get_rpm(velocity_measure);
	monitor_frame.velocity_setpoint = mc_mngr_instance.velocity_setpoint;

	uart_transmit(uart, (const uint8_t*)&monitor_frame, sizeof(foc_monitor_frame_t));
}


static void velocity_measure_substask(mc_callback_param_t param)
{
	UNUSED(param);
	float_t observer_electrical_speed = sliding_mode_observer_get_electrical_speed(smo);
	velocity_measure_process(velocity_measure, observer_electrical_speed);
}

static void current_transformation_subtask(mc_callback_param_t param)
{
	UNUSED(param);

	current_measure_process(current_measure);
	current_measure_act_curr_t curr_abc = current_measure_get_currents(current_measure);
	current_transformation_process(current_transformation, curr_abc.curr_a,  curr_abc.curr_b,  curr_abc.curr_c, mc_mngr_instance.theta_ref);
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
		mc_mngr_instance.vd_setpoint = curr_cntrl_output.vd;
		mc_mngr_instance.vq_setpoint = curr_cntrl_output.vq;
		sv_modulation_set_target_vd_vq(sv_modulation, mc_mngr_instance.vd_setpoint, mc_mngr_instance.vq_setpoint);
	}
}

static void modulation_subtask(mc_callback_param_t param)
{
	UNUSED(param);
	sv_modulation_process(sv_modulation, mc_mngr_instance.theta_ref);
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


void motor_control_manager_init(motor_control_manager_cfg_t const *cfg)
{
	mc_mngr_instance.cfg = *cfg;
	mc_mngr_instance.mc_direction = MC_DIR_CCW;
	mc_mngr_instance.mc_state = MC_STATE_IDLE;

	mc_mngr_instance.sampling_time = (1.0f / mc_mngr_instance.cfg.pwm_freq);
	mc_mngr_instance.time_base = ONE_SECOND_IN_NANOSECONDS / mc_timer_get_freq(mc_timer);
	mc_mngr_instance.theta_ref_log = 0.0f;
	mc_mngr_instance.theta_ref = 0.0f;
	mc_mngr_instance.omega_ref = 0.0f;
	mc_mngr_instance.velocity_setpoint = 0.0f;
	mc_mngr_instance.id_setpoint = 0.0f;
	mc_mngr_instance.vq_setpoint = 0.0f;

	mc_mngr_instance.aligment_ticks = (uint32_t)(((float_t)mc_mngr_instance.cfg.alignment_time * mc_mngr_instance.cfg.pwm_freq) / 1000.0f);
	mc_mngr_instance.aligment_tick_counter = 0u;

	mc_mngr_instance.open_loop_ramp_time_ticks = (uint32_t)(((float_t)mc_mngr_instance.cfg.open_loop_ramp_time * mc_mngr_instance.cfg.pwm_freq) / 1000.0f);
	mc_mngr_instance.open_loop_ramp_time_tick_counter = 0u;
	uint32_t pole_pairs = motor_cfg_get_motor_pole_pairs();
	float_t temp_elec_freq_hz = ((float_t)((mc_mngr_instance.cfg.open_loop_velocity_setpoint * pole_pairs * 2u) / 120.0f));
	mc_mngr_instance.open_loop_omega_max = temp_elec_freq_hz * CONSTANT_TWO_PI;
	mc_mngr_instance.open_loop_omega_dt = (mc_mngr_instance.open_loop_omega_max / (mc_mngr_instance.cfg.open_loop_ramp_time / 1000.0f));
	mc_mngr_instance.theta_open_loop = 0.0f;
	mc_mngr_instance.omega_open_loop = 0.0f;
	mc_mngr_instance.omega_coeff = get_omega_coeff(mc_mngr_instance.mc_direction);
	mc_mngr_instance.velocity_setpoint = mc_mngr_instance.omega_coeff * mc_mngr_instance.cfg.open_loop_velocity_setpoint;

	mc_mngr_instance.transition_time_ticks = (uint32_t)(((float_t)mc_mngr_instance.cfg.transition_time * mc_mngr_instance.cfg.pwm_freq) / 1000.0f);
	mc_mngr_instance.transition_time_tick_counter = 0u;
	mc_mngr_instance.theta_offset = 0.0f;
	mc_mngr_instance.theta_offset_arr_idx = 0u;
	mc_mngr_instance.theta_offset_arr_size = sizeof(mc_mngr_instance.theta_offset_arr) / sizeof(float_t);
	mc_mngr_instance.theta_offset_calculated = FALSE;


	motor_control_manager_register_subtask(&mc_mngr_instance.task_1_subtasks[MC_TASK_1_COMMUNICATION], MOTION_CONTROL_MANAGER_CYCLE_TIME_10000, communication_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_1_subtasks[MC_TASK_1_STATE_MACHINE], MOTION_CONTROL_MANAGER_CYCLE_TIME_62_50, state_machine_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_CURRENT_TRANSFORMATION], MOTION_CONTROL_MANAGER_CYCLE_TIME_62_50, current_transformation_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_OBSERVER], MOTION_CONTROL_MANAGER_CYCLE_TIME_62_50, observer_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_VELOCITY_MEASURE], MOTION_CONTROL_MANAGER_CYCLE_TIME_62_50, velocity_measure_substask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_VELOCITY_CONTROL], MOTION_CONTROL_MANAGER_CYCLE_TIME_250, velocity_control_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_CURRENT_CONTROL], MOTION_CONTROL_MANAGER_CYCLE_TIME_62_50, current_controller_subtask, NULL);
	motor_control_manager_register_subtask(&mc_mngr_instance.task_2_subtasks[MC_TASK_2_MODULATION], MOTION_CONTROL_MANAGER_CYCLE_TIME_62_50, modulation_subtask, NULL);

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
			sliding_mode_observer_reset(smo);
			current_controller_set_target_id(current_controller, 0.0f);
			current_controller_set_target_iq(current_controller, 0.0f);
			velocity_controller_set_target_velocity(velocity_controller, 0u);
			current_controller_reset(current_controller);
			velocity_controller_reset(velocity_controller);
			sv_modulation_set_target_vd_vq(sv_modulation, 0.0f, 0.0f);
			mc_mngr_instance.mc_state = MC_STATE_IDLE;
		}
		else if(mc_mngr_instance.mc_state == MC_STATE_IDLE)
		{
			mc_mngr_instance.mc_state = MC_STATE_ALIGN;
		}
	}
}

