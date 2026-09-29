#include <motor_cfg.h>
#include <motor.h>

static motor_cfg_t motor_config = {
	.motor_nom_voltage = MOTOR_NOM_VOLTAGE_MV,
	.motor_max_current = MOTOR_MAX_CURRENT_MA,
	.motor_stator_resistance = MOTOR_RESISTANCE_MOHM,
	.motor_stator_inductance = MOTOR_INDUCTANCE_MHENRY,
	.max_rpm = MOTOR_MAX_RPM,
	.kv_value = MOTOR_KV_VALUE,
	.pole_pairs = MOTOR_POLE_PAIRS
};

float_t motor_cfg_get_motor_nom_voltage(void)
{
    float_t ret_val;
    ret_val = motor_config.motor_nom_voltage;
    return ret_val;
}

float_t motor_cfg_get_motor_max_current(void)
{
    float_t ret_val;
    ret_val = motor_config.motor_max_current;
    return ret_val;
}

float_t motor_cfg_get_motor_stator_resistance(void)
{
    float_t ret_val;

    ret_val = motor_config.motor_stator_resistance;

    return ret_val;
}

float_t motor_cfg_get_motor_stator_inductance(void)
{
    float_t ret_val;
    ret_val = motor_config.motor_stator_inductance;
    return ret_val;
}

uint32_t motor_cfg_get_motor_max_rpm(void)
{
	uint32_t ret_val;
    ret_val = motor_config.max_rpm;
    return ret_val;
}

uint32_t motor_cfg_get_motor_kv_value(void)
{
    uint32_t ret_val;
    ret_val = motor_config.kv_value;
    return ret_val;
}

uint32_t motor_cfg_get_motor_pole_pairs(void)
{
    uint32_t ret_val;
    ret_val = motor_config.pole_pairs;
    return ret_val;
}
