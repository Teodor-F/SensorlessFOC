#ifndef MOTOR_CFG_H
#define MOTOR_CFG_H

typedef struct motor_cfg motor_cfg_t;

struct motor_cfg {
	float_t motor_nom_voltage;			//[mV]
	float_t motor_max_current;			//[mA]
	float_t motor_stator_resistance;	//[mOhm]
	float_t motor_stator_inductance;	//[mH]
	uint32_t max_rpm;					//[rpm]
	uint32_t kv_value;					//[rpm/v]
	uint32_t pole_pairs;				//[no unit]
};

float_t motor_cfg_get_motor_nom_voltage(void);
float_t motor_cfg_get_motor_max_current(void);
float_t motor_cfg_get_motor_stator_resistance(void);
float_t motor_cfg_get_motor_stator_inductance(void);
uint32_t motor_cfg_get_motor_max_rpm(void);
uint32_t motor_cfg_get_motor_kv_value(void);
uint32_t motor_cfg_get_motor_pole_pairs(void);


#endif /* MOTOR_CFG_H */
