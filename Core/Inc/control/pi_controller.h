#ifndef PI_CNTROLLER_H_
#define PI_CNTROLLER_H_

typedef struct pi_controller_cfg pi_controller_cfg_t;
typedef struct pi_controller pi_controller_t;

struct pi_controller_cfg {
	float kp;
	float ki;
	float ts;
	float out_limit;
};

struct pi_controller {
    float kp;
    float ki;
    float ts;
    float ki_ts_by_two;
    float integral;
    float prev_error;
    float output_max;
    float output_min;
    float output;
};

void pi_controller_init(pi_controller_t* const instance, const pi_controller_cfg_t* const cfg);

float pi_controller_process(pi_controller_t* const instance, float error);

void pi_controller_reset(pi_controller_t* const instance);

#endif /* PI_CNTROLLER_H_ */
