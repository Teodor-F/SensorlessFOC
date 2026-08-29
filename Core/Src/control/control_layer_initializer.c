#include <control_layer_initializer.h>

#include <motor_cfg.h>

velocity_controller_t *velocity_controller = NULL;
static velocity_controller_t velocity_cntrl_instance = {0};

current_controller_t *current_controller = NULL;
static current_controller_t curr_cntrl_instance = {0};

sv_modulation_t *sv_modulation = NULL;
static sv_modulation_t sv_modulation_istance = {0};

lpf_first_order_t *lpf_id = NULL;
static lpf_first_order_t lpf_id_instance = {0};

lpf_first_order_t *lpf_iq = NULL;
static lpf_first_order_t lpf_iq_istance = {0};

void control_layer_initializer(void)
{
	lpf_first_order_cfg_t lpf_id_iq_cfg = {
		.cutoff_freq_hz = 10u,
		.ts = 1.0f / PWM_FREQ_HZ
	};
	lpf_first_order_init(&lpf_id_instance, &lpf_id_iq_cfg);
	lpf_id = &lpf_id_instance;
	lpf_first_order_init(&lpf_iq_istance, &lpf_id_iq_cfg);
	lpf_iq = &lpf_iq_istance;

//========================================================================
	// Velocity controller initialization
	uint32_t motor_max_rpm = motor_cfg_get_motor_max_rpm();
	velocity_controller_cfg_t velocity_cntrl_cfg = {
		.kp = 0.025,
		.ki = 85.0f,
		.velocity_ref_limit = motor_max_rpm,
		.iq_max_out = 1250.0f,
		.ts = 1.0f / PWM_FREQ_HZ
	};
	velocity_controller_init(&velocity_cntrl_instance, &velocity_cntrl_cfg);
	velocity_controller = &velocity_cntrl_instance;

//========================================================================
	// Current controller initialization
	float_t motor_nominal_voltage = motor_cfg_get_motor_nom_voltage();
	float_t motor_stator_resistance = motor_cfg_get_motor_stator_resistance();
	float_t motor_stator_inductance =  motor_cfg_get_motor_stator_inductance();
	float_t motor_max_current = motor_cfg_get_motor_max_current();
	current_controller_cfg_t curr_cntrl_cfg = {
		.kp_id = 0.085,
		.ki_id = 170.0f,
		.kp_iq = 0.085,
		.ki_iq = 170.0f,
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
