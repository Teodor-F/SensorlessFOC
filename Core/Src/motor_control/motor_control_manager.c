#include <motor_control_manager.h>
#include <control_layer_initializer.h>
#include <measurement_layer_initializer.h>
#include <periph_layer_initializer.h>

#include <sv_transformations.h>
#include <motor_cfg.h>
#include <foc_monitor.h>


//========================================================================
	// Communication layer
volatile bool foc_telemetry_ready = false;
volatile foc_monitor_frame_t monitor_frame = {0};


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

static float_t calculate_theta_offset(void)
{
	float_t ret_val = 0.0f;
	float_t sum = 0.0f;
	for (uint32_t idx = 0; idx < mc_mngr_instance.theta_offset_arr_size; ++idx)
	{
		sum += mc_mngr_instance.theta_offset_arr[idx];
	}
	ret_val = sum /=  mc_mngr_instance.theta_offset_arr_size;
	return ret_val;
}

static void communication_task(mc_callback_param_t param)
{
	(void)param;
	static uint32_t uart_comm_sw_delay = 0u;
	if(uart_comm_sw_delay == UART_COMM_TRIG_CNT_VALUE)
	{
		uart_comm_sw_delay = 0u;

		current_measure_act_curr_t curr_abc = current_measure_get_currents(current_measure);
		float_t curr_alpha;
		float_t curr_beta;
		sv_clarke_transform(curr_abc.curr_a, curr_abc.curr_b, curr_abc.curr_c, &curr_alpha, &curr_beta);
		float_t curr_d;
		float_t curr_q;
		sv_park_transform(curr_alpha, curr_beta, sinf(mc_mngr_instance.theta_ref), cosf(mc_mngr_instance.theta_ref), &curr_d, &curr_q);
		lpf_first_order_process(lpf_id, curr_d);
		lpf_first_order_process(lpf_iq, curr_q);

		sliding_mode_observer_emf_est_t emf_alpha_beta = sliding_mode_observer_get_emfs(smo);
		float_t theta_smo = sliding_mode_observer_get_electrical_angle(smo);

		monitor_frame.header = FOC_FRAME_HEADER;
		monitor_frame.ia_mA = curr_abc.curr_a;
		monitor_frame.ib_mA = curr_abc.curr_b;
		monitor_frame.ic_mA = curr_abc.curr_c;
		monitor_frame.id_mA = lpf_first_order_get_filtered_value(lpf_id);
		monitor_frame.iq_mA = lpf_first_order_get_filtered_value(lpf_iq);
		monitor_frame.emf_alpha = emf_alpha_beta.emf_alfa;
		monitor_frame.emf_beta = emf_alpha_beta.emf_beta;
		monitor_frame.theta_real_rad = mc_mngr_instance.theta_open_loop;
		monitor_frame.theta_pll_rad = theta_smo;
		monitor_frame.theta_ref_log_rad = mc_mngr_instance.theta_ref_log;
		monitor_frame.velocity_pll_rpm = velocity_measure_get_rpm(velocity_measure);
		monitor_frame.velocity_setpoint = mc_mngr_instance.velocity_setpoint;
		foc_telemetry_ready = true;
	}
	uart_comm_sw_delay++;
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
			current_controller_set_target_id(current_controller, 0.0f);
			current_controller_set_target_iq(current_controller, mc_mngr_instance.cfg.aligment_iq);
			mc_mngr_instance.aligment_tick_counter++;
			if(mc_mngr_instance.aligment_tick_counter >= mc_mngr_instance.aligment_ticks)
			{
				mc_mngr_instance.aligment_tick_counter = 0u;
				mc_mngr_instance.theta_open_loop = 0.0f;
				mc_mngr_instance.omega_open_loop = 0.0f;
				current_controller_set_target_iq(current_controller, 0.0f);
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
				mc_mngr_instance.theta_offset_arr_idx = 0u;
				mc_mngr_instance.transition_time_tick_counter = 0u;
				mc_mngr_instance.theta_offset_calculated = FALSE;
				mc_mngr_instance.mc_state = MC_STATE_TRANSITION;
			}

			mc_mngr_instance.theta_open_loop += mc_mngr_instance.omega_coeff * mc_mngr_instance.omega_open_loop * mc_mngr_instance.sampling_time;
			CONSTRAIN_ANGLE_RAD_ZERO_TWO_PI(mc_mngr_instance.theta_open_loop);
			mc_mngr_instance.theta_ref = mc_mngr_instance.theta_open_loop;
			mc_mngr_instance.theta_ref_log = mc_mngr_instance.theta_ref;

			break;
		}
//========================================================================
		// Let the observer stabilize for a predefined time
		case MC_STATE_TRANSITION:
		{
			if(mc_mngr_instance.theta_offset_calculated == FALSE)
			{

				mc_mngr_instance.theta_open_loop += mc_mngr_instance.omega_coeff * mc_mngr_instance.omega_open_loop * mc_mngr_instance.sampling_time;
				CONSTRAIN_ANGLE_RAD_ZERO_TWO_PI(mc_mngr_instance.theta_open_loop);
				mc_mngr_instance.theta_ref = mc_mngr_instance.theta_open_loop;
				mc_mngr_instance.transition_time_tick_counter++;

				if(mc_mngr_instance.transition_time_tick_counter >= mc_mngr_instance.transition_time_ticks)
				{
					mc_mngr_instance.id_setpoint = 0.0f;
					mc_mngr_instance.iq_setpoint = 200.0f;
					mc_mngr_instance.mc_state = MC_STATE_CLOSED_LOOP;
				}
			}
			break;
		}
		case MC_STATE_CLOSED_LOOP:
		{
			current_controller_set_target_id(current_controller, mc_mngr_instance.id_setpoint);
			current_controller_set_target_iq(current_controller, mc_mngr_instance.iq_setpoint);
			float_t theta = pll_get_est_theta(pll);
			mc_mngr_instance.theta_ref = theta;
			break;
		}
		case MC_STATE_ERROR:
		{
			break;
		}
	}
}

static void motor_control_task(mc_callback_param_t param)
{
	(void)param;

//========================================================================
	// 0. Measure and control velocity
	velocity_measure_process(velocity_measure, pll_get_est_omega(pll));
	velocity_controller_process(velocity_controller, velocity_measure_get_rpm(velocity_measure));

//========================================================================
	// 1. Convert ADC current values to milliamps
	current_measure_process(current_measure);
	current_measure_act_curr_t curr_abc = current_measure_get_currents(current_measure);
//========================================================================
	// 2. Clarke- & Park Transformation
	float_t curr_alpha;
	float_t curr_beta;
	sv_clarke_transform(curr_abc.curr_a, curr_abc.curr_b, curr_abc.curr_c, &curr_alpha, &curr_beta);
	float_t curr_d;
	float_t curr_q;
	sv_park_transform(curr_alpha, curr_beta, sinf(mc_mngr_instance.theta_ref), cosf(mc_mngr_instance.theta_ref), &curr_d, &curr_q);

//========================================================================
	// 3. Sliding mode observer & PLL
	float_t v_alfa;
	float_t v_beta;
	sv_modulation_get_v_alfa_v_beta(sv_modulation, &v_alfa, &v_beta);
	sliding_mode_observer_process(smo, v_alfa, v_beta, curr_alpha, curr_beta);
	sliding_mode_observer_emf_est_t smo_emfs = sliding_mode_observer_get_emfs(smo);
	pll_process_new(pll, smo_emfs.emf_alfa, smo_emfs.emf_beta);

//========================================================================
	// 4. Execute state machine
	motor_control_state_machine();

//========================================================================
	// 5. Apply Current controller in dq-frame
	if(mc_mngr_instance.mc_state != MC_STATE_IDLE && mc_mngr_instance.mc_state != MC_STATE_ERROR)
	{
		current_controller_process(current_controller, curr_d, curr_q);
		current_controller_output_t curr_cntrl_output = current_controller_get_vd_vq_out(current_controller);
		mc_mngr_instance.vd_setpoint = curr_cntrl_output.vd;
		mc_mngr_instance.vq_setpoint = curr_cntrl_output.vq;
	}

//	5. Execute modulation
	sv_modulation_set_target_vd_vq(sv_modulation, mc_mngr_instance.vd_setpoint, mc_mngr_instance.vq_setpoint);
	sv_modulation_process(sv_modulation, mc_mngr_instance.theta_ref);
}


void motor_control_manager_init(motor_control_manager_cfg_t const *cfg)
{
	mc_mngr_instance.cfg = *cfg;
	mc_mngr_instance.mc_direction = MC_DIR_CW;
	mc_mngr_instance.mc_state = MC_STATE_IDLE;

	mc_mngr_instance.sampling_time = (1.0f / mc_mngr_instance.cfg.pwm_freq);
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

	mc_timer_register_mc_callback(MCTIMER_CB_IDX_1, communication_task, NULL);
	mc_timer_register_mc_callback(MCTIMER_CB_IDX_2, motor_control_task, NULL);
	mc_timer_activate_callback(MCTIMER_CB_IDX_1);
	mc_timer_activate_callback(MCTIMER_CB_IDX_2);

	// Synchronize timers
	mc_timer_start();
	pwm_start();
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
			current_controller_reset(current_controller);
			mc_mngr_instance.mc_state = MC_STATE_IDLE;
		}
		else if(mc_mngr_instance.mc_state == MC_STATE_IDLE)
		{
			mc_mngr_instance.mc_state = MC_STATE_ALIGN;
		}
	}
}

