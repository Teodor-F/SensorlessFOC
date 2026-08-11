#include <motor_control.h>

#include <adc.h>
#include <pwm.h>
#include <mctimer.h>
#include <hall.h>

#include <current_measure.h>
#include <sliding_mode_observer.h>
#include <pll.h>

#include <sv_modulation.h>
#include <pi_cntrl.h>
#include <velocity_measure.h>

#include <sv_transformations.h>
#include <foc_monitor.h>

typedef enum motor_controller_state motor_controller_state_t;
typedef struct motor_controller motor_controller_t;

enum motor_controller_state {
	MC_STATE_IDLE,
	MC_STATE_ALIGN,
	MC_STATE_OPEN_LOOP,
	MC_STATE_STABILIZATION,
	MC_STATE_SENSORLESS,
	MC_STATE_ERROR
};

//========================================================================
	// Measurement layer instances
static current_measure_t curr_meas = {0};
static sliding_mode_observer_t smo = {0};
static pll_t pll = {0};

//========================================================================
	// Motor control layer instances
static sv_modulation_t modulator = {0};
static pi_cntrl_t pi_cntrl_curr_d = {0};
static pi_cntrl_t pi_cntrl_curr_q = {0};

//========================================================================
	// Motor control layer
static motor_controller_state_t mc_state = MC_STATE_IDLE;
static float theta = 0.0f;
static uint32_t aligment_tick_counter = 0u;
static float omega_open_loop = 0.0f;
static float theta_open_loop = 0.0f;
static float theta_error = 0.0f;
static float target_rpm = 400.0f;
//========================================================================
	// Communication layer
volatile bool foc_telemetry_ready = false;
volatile foc_monitor_frame_t monitor_frame = {0};

static void periphs_layer_initializer(void)
{
//========================================================================
	// ADC initialization
	adc_init();

//========================================================================
	// PWM-signal generator initialization
	pwm_init();

//========================================================================
	// Motor control timer initialization
	mc_timer_init();

//========================================================================
	// Hall sensor initialization
	hall_init();
}

static void measurement_layer_initializer(void)
{

//========================================================================
	// Current measurement initialization
	current_measure_config_t curr_meas_cfg = {
			.adc_max_value = 4095u,
			.adc_ref_voltage = 3300.0f,
			.gain = 5.1818f,
			.offset_voltage = 1710.0f,
			.shunt_value = 0.01f
	};
	current_measure_init(&curr_meas, &curr_meas_cfg);

//========================================================================

	sliding_mode_observer_cfg_t smo_cfg = {
		.rs = MOTOR_RESISTANCE_MOHM,
		.ls = MOTOR_INDUCTANCE_MHENRY,
		.ts = 1.0f / (float_t)PWM_FREQ_HZ,
		.boundary = 150.0f,
		.k_sliding_gain = 50.0f,
	    .g_emf_gain = 0.085f,
		.omega_lpf_gain = 0.035
	};

	sliding_mode_observer_init(&smo, &smo_cfg);

//========================================================================
	// Phase-locked-loop initialization
	pll_cfg_t pll_cfg = {
			.kp = 30.0f,
		    .ki = 600.0f,
		    .ts =  1.0f / (float_t)PWM_FREQ_HZ,
		    .omega_max = 4000.0f,
	};
	pll_init(&pll, &pll_cfg);
}

static void control_layer_initializer(void)
{
//========================================================================
	// Space vector modulation initialization
	sv_modulation_cfg_t sv_modulation_cfg = {
		.v_bus = MOTOR_NOM_VOLTAGE_MV
	};
	sv_modulation_init(&modulator, &sv_modulation_cfg);

//========================================================================
	// Direct-current PI-controller initialization
	pi_cntrl_cfg_t pi_cntrl_curr_d_cfg = {0};
	pi_cntrl_init(&pi_cntrl_curr_d, &pi_cntrl_curr_d_cfg);

//========================================================================
	// Quadrature-current PI-controller initialization
	pi_cntrl_cfg_t pi_cntrl_curr_q_cfg = {0};
	pi_cntrl_init(&pi_cntrl_curr_q, &pi_cntrl_curr_q_cfg);
}

static void init_builtin_led(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void init_user_button(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

static void communication_task(mc_callback_param_t param)
{
	(void)param;
	static uint32_t uart_comm_sw_delay = 0u;
	if(uart_comm_sw_delay == UART_COMM_TRIG_CNT_VALUE)
	{
		uart_comm_sw_delay = 0u;

		current_measure_act_curr_t curr_abc = current_measure_get_currents(&curr_meas);
		sliding_mode_observer_emf_est_t emf_alpha_beta = sliding_mode_observer_get_emfs(&smo);
		float theta_smo = sliding_mode_observer_get_electrical_angle(&smo);
		float omega_smo = sliding_mode_observer_get_electrical_speed(&smo);

		monitor_frame.header = FOC_FRAME_HEADER;
		monitor_frame.ia_mA = curr_abc.curr_a;
		monitor_frame.ib_mA = curr_abc.curr_b;
		monitor_frame.ic_mA = curr_abc.curr_c;
		monitor_frame.emf_alpha = emf_alpha_beta.emf_alfa;
		monitor_frame.emf_beta = emf_alpha_beta.emf_beta;

		float theta_err = theta_smo - theta_open_loop;
		while(theta_err > CONSTANT_PI)
		{
			theta_err -= CONSTANT_TWO_PI;
		}
		while(theta_err < -CONSTANT_PI)
		{
			theta_err += CONSTANT_TWO_PI;
		}

		monitor_frame.theta_pll_rad = theta_err;
		monitor_frame.theta_real_rad = 0.0f;
		monitor_frame.omega_pll_rad_s = omega_smo;
		monitor_frame.omega_real_rad_s = omega_open_loop;
		foc_telemetry_ready = true;
	}
	uart_comm_sw_delay++;
}

static void motor_control_task(mc_callback_param_t param)
{
	(void)param;
//========================================================================
	// 1. Convert ADC current values to milliamps
	current_measure_process(&curr_meas);
	current_measure_act_curr_t curr_abc = current_measure_get_currents(&curr_meas);
//========================================================================
	// 2. Clarke-transform
	float curr_alpha;
	float curr_beta;
	sv_clarke_transform(curr_abc.curr_a, curr_abc.curr_b, curr_abc.curr_c, &curr_alpha, &curr_beta);
//========================================================================
	// 3. Sliding mode observer
	sliding_mode_observer_process(&smo, modulator.v_alfa,modulator.v_beta, curr_alpha, curr_beta);
	float theta_smo = sliding_mode_observer_get_electrical_angle(&smo);
	float omega_smo = sliding_mode_observer_get_electrical_speed(&smo);

	switch (mc_state)
	{
		case MC_STATE_ALIGN:
		{
			sv_modulation_set_target_vd_vq(&modulator, 0.0f, 800.0f);
			theta = 0.0f;
			aligment_tick_counter++;
			if (aligment_tick_counter >= MOTOR_ALIGMENT_TIME_TICKS)
			{
				aligment_tick_counter = 0u;
				theta = 0.0f;
				theta_open_loop = 0.0f;
				omega_open_loop = 0.0f;
				mc_state = MC_STATE_OPEN_LOOP;
			}
			break;
		}
		case MC_STATE_OPEN_LOOP:
		{
			sv_modulation_set_target_vd_vq(&modulator, 0.0f, 1000.0f);
			omega_open_loop += MOTOR_OPEN_LOOP_ACCELERATION_RAD_S2 * TS;

			if (omega_open_loop >= MOTOR_OPEN_LOOP_TARGET_ELEC_SPEED_RAD_S)
			{
				omega_open_loop = MOTOR_OPEN_LOOP_TARGET_ELEC_SPEED_RAD_S;
			}
			theta_open_loop -= omega_open_loop * TS;
			if (theta_open_loop <= 0.0)
			{
				theta_open_loop += CONSTANT_TWO_PI;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	sv_modulation_process(&modulator, theta_open_loop);
}


void motor_control_initializer(void)
{
	init_builtin_led();
	init_user_button();

	periphs_layer_initializer();
	measurement_layer_initializer();
	control_layer_initializer();

	mc_timer_register_mc_callback(MCTIMER_CB_IDX_1, communication_task, NULL);
	mc_timer_register_mc_callback(MCTIMER_CB_IDX_2, motor_control_task, NULL);
	mc_timer_activate_callback(MCTIMER_CB_IDX_1);
	mc_timer_activate_callback(MCTIMER_CB_IDX_2);

	mc_timer_start();
	pwm_start();

	//sv_modulation_set_target_vd_vq(&modulator, 0.0f, 1200.0f);
}

void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_13)
	{
		mc_state = MC_STATE_ALIGN;
	}
}
