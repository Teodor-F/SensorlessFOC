#include <motor_control_manager.h>

void motor_control_manager_initializer(void)
{
	motor_control_manager_cfg_t mc_mngr_cfg = {
		.pwm_freq = PWM_FREQ_HZ,
		.alignment_time = 50u,
		.aligment_id = 0.0f,
		.aligment_iq = 1500,
		.open_loop_ramp_time = 400u,
		.open_loop_velocity_setpoint = 400u,
		.open_loop_id = 0.0f,
		.open_loop_iq = 1500,
		.transition_time = 300u,
		.transition_iq = 900.0f
	};

	motor_control_manager_init(&mc_mngr_cfg);
}
