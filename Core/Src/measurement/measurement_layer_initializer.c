#include <measurement_layer_initializer.h>
#include <motor_cfg.h>

velocity_measure_t *velocity_measure = NULL;
current_measure_t *current_measure = NULL;
sliding_mode_observer_t *smo = NULL;

//========================================================================
	// Measurement layer instances
static lpf_first_order_t velocity_measure_lpf = {0};
static velocity_measure_t vel_meas_instance = {0};
static current_measure_t curr_meas_instance = {0};
static sliding_mode_observer_t smo_instance = {0};

void measurement_layer_initializer(void)
{
	uint32_t motor_max_rpm = motor_cfg_get_motor_max_rpm();
	uint32_t motor_pole_pairs = motor_cfg_get_motor_pole_pairs();
	uint32_t motor_kv_value = motor_cfg_get_motor_kv_value();
	float_t motor_resistance = motor_cfg_get_motor_stator_resistance();
	float_t motor_inductance = motor_cfg_get_motor_stator_inductance();

//========================================================================
	// Velocity measurement initialization
	lpf_first_order_cfg_t velocity_measure_lpf_cfg = {
		.cutoff_freq_hz = 10.0f,
		.cycle_time = VELOCITY_MEASURE_CYCYLE_TIME
	};
	lpf_first_order_init(&velocity_measure_lpf, &velocity_measure_lpf_cfg);

	velocity_measure_config_t vel_meas_cfg = {
		.kv_value = motor_kv_value,
		.pole_pairs = motor_pole_pairs,
		.lpf_fo_instance = &velocity_measure_lpf
	};
	velocity_measure_init(&vel_meas_instance, &vel_meas_cfg);


//========================================================================
	// Current measurement initialization
	current_measure_config_t curr_meas_cfg = {
			.adc_max_value = 4095u,
			.adc_ref_voltage = 3300.0f,
			.gain = 5.1818f,
			.offset_voltage = 1710.0f,
			.shunt_value = 0.01f
	};
	current_measure_init(&curr_meas_instance, &curr_meas_cfg);

//========================================================================
	// Sliding mode observer initialization
	sliding_mode_observer_cfg_t smo_cfg = {
		.rs = motor_resistance,
		.ls = motor_inductance,
		.boundary = 175.0f,
		.k_sliding_gain = 60.0f,
		.g_emf_gain = 0.065f,
		.emf_cntr_threshold = 500u,
		.emf_threshold = 500.0f,
		.cycle_time = OBSERVER_CYCLE_TIME
	};
	sliding_mode_observer_init(&smo_instance, &smo_cfg);


//========================================================================
	// Init global pointers
	velocity_measure = &vel_meas_instance;
	current_measure = &curr_meas_instance;
	smo = &smo_instance;
}

