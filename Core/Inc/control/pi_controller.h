#ifndef PI_CNTROLLER_H_
#define PI_CNTROLLER_H_

typedef struct pi_controller_cfg pi_controller_cfg_t;
typedef struct pi_controller pi_controller_t;

struct pi_controller_cfg {
	float_t kp;
	float_t ki;
	float_t out_limit;
	motor_control_cycle_time_t cycle_time;

};

struct pi_controller {
    float_t kp;
    float_t ki;
    float_t sampling_time;
    float_t ki_ts_by_two;
    float_t integral;
    float_t prev_error;
    float_t output_max;
    float_t output_min;
    float_t output;
};

void pi_controller_init(pi_controller_t* const instance, const pi_controller_cfg_t* const cfg);

float_t pi_controller_process(pi_controller_t* const instance, float_t error);

void pi_controller_set_integral(pi_controller_t* const instance, float_t integral);

void pi_controller_reset(pi_controller_t* const instance);

#endif /* PI_CNTROLLER_H_ */
