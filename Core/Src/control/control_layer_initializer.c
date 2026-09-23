#include <control_layer_initializer.h>
#include <motor_cfg.h>

velocity_controller_t *velocity_controller = NULL;
static velocity_controller_t velocity_cntrl_instance = {0};

revup_controller_t *revup_controller = NULL;
static revup_controller_t revup_cntlr_instance = {0};

current_transformation_t *current_transformation = NULL;
static current_transformation_t curr_trans_instance = {0};

current_controller_t *current_controller = NULL;
static current_controller_t curr_cntrl_instance = {0};

sv_modulation_t *sv_modulation = NULL;
static sv_modulation_t sv_modulation_istance = {0};

lpf_first_order_t *lpf_i_alfa = NULL;
static lpf_first_order_t lpf_i_alfa_instance = {0};

lpf_first_order_t *lpf_i_beta = NULL;
static lpf_first_order_t lpf_i_beta_instance = {0};

lpf_first_order_t *lpf_id = NULL;
static lpf_first_order_t lpf_id_instance = {0};

lpf_first_order_t *lpf_iq = NULL;
static lpf_first_order_t lpf_iq_istance = {0};

void control_layer_initializer(void)
{

//========================================================================
	// Velocity controller initialization
	uint32_t motor_max_rpm = motor_cfg_get_motor_max_rpm();
	velocity_controller_cfg_t velocity_cntrl_cfg = {
		.kp = 1.25f,
		.ki = 3.75f,
		.velocity_ref_limit = motor_max_rpm,
		.iq_max_out = 1000.0f,
		.ts = 0.00025f
	};
	velocity_controller_init(&velocity_cntrl_instance, &velocity_cntrl_cfg);
	velocity_controller = &velocity_cntrl_instance;

//========================================================================
	// I/f Revup controller initialization
	revup_controller_config_t revup_cntrl_cfg = {
		.pwm_freq = PWM_FREQ_HZ,
		.alignment_time = 100u,
		.aligment_id = 0.0f,
		.aligment_iq = 1000.0f,
		.open_loop_ramp_time = 500u,
		.open_loop_velocity_setpoint = 500u,
		.open_loop_id = 0.0f,
		.open_loop_iq = 750.0f,
		.stabilization_time = 500u,
		.stabilization_id = 0.0f,
		.stabilization_iq = 225.0f
	};
	revup_controller_init(&revup_cntlr_instance, &revup_cntrl_cfg);
	revup_controller = &revup_cntlr_instance;

//========================================================================
	// Current transformation initialization
	current_transformation_init(&curr_trans_instance);
	current_transformation = &curr_trans_instance;

//========================================================================
	// Current controller initialization
	lpf_first_order_cfg_t lp_i_alfa_beta = {
		.cutoff_freq_hz = 1000u,
		.ts = 1.0f / PWM_FREQ_HZ
	};

	lpf_first_order_cfg_t lpf_i_dq_cfg = {
		.cutoff_freq_hz = 50u,
		.ts = 1.0f / PWM_FREQ_HZ
	};

	lpf_first_order_init(&lpf_i_alfa_instance, &lp_i_alfa_beta);
	lpf_first_order_init(&lpf_i_beta_instance, &lp_i_alfa_beta);
	lpf_first_order_init(&lpf_id_instance, &lpf_i_dq_cfg);
	lpf_first_order_init(&lpf_iq_istance, &lpf_i_dq_cfg);

	lpf_i_alfa = &lpf_i_alfa_instance;
	lpf_i_beta = &lpf_i_beta_instance;
	lpf_id = &lpf_id_instance;
	lpf_iq = &lpf_iq_istance;


	float_t motor_nominal_voltage = motor_cfg_get_motor_nom_voltage();
	float_t motor_stator_resistance = motor_cfg_get_motor_stator_resistance();
	float_t motor_stator_inductance =  motor_cfg_get_motor_stator_inductance();
	float_t motor_max_current = motor_cfg_get_motor_max_current();
	current_controller_cfg_t curr_cntrl_cfg = {
		.kp_id = 0.045,
		.ki_id = 85.0f,
		.kp_iq = 0.045,
		.ki_iq = 85.0f,
		.vd_out_limit = motor_nominal_voltage * ONE_BY_SQRT_THREE,
		.vq_out_limit =  motor_nominal_voltage * ONE_BY_SQRT_THREE,
		.ts = 1.0f / PWM_FREQ_HZ,
	};
	current_controller_init(&curr_cntrl_instance, &curr_cntrl_cfg);
	current_controller = &curr_cntrl_instance;

//========================================================================
	// Space vector modulation initialization
	float_t motor_nom_voltage = motor_cfg_get_motor_nom_voltage();
	sv_modulation_cfg_t sv_modulation_cfg = {
		.v_bus = motor_nom_voltage,
	};
	sv_modulation_init(&sv_modulation_istance, &sv_modulation_cfg);

	sv_modulation = &sv_modulation_istance;
}
