#include <motor_control_manager.h>

void motor_control_manager_initializer(void)
{
	motor_control_manager_cfg_t mc_mngr_cfg = {
		.pwm_freq = PWM_FREQ_HZ,
		.alignment_time = 100u,
		.aligment_id = 0.0f,
		.aligment_iq = 1000.0f,
		.open_loop_ramp_time = 600u,
		.open_loop_velocity_setpoint = 600u,
		.open_loop_id = 0.0f,
		.open_loop_iq = 800.0f,
		.transition_time = 500u,
		.transition_iq = 300.0f
	};

	motor_control_manager_init(&mc_mngr_cfg);
}
