#ifndef LPF_FIRST_ORDER_H_
#define	LPF_FIRST_ORDER_H_

typedef struct lpf_first_order_cfg lpf_first_order_cfg_t;
typedef struct lpf_first_order lpf_first_order_t;

struct lpf_first_order_cfg {
	motor_control_cycle_time_t cycle_time;
	float_t cutoff_freq_hz;
};

struct lpf_first_order {
	lpf_first_order_cfg_t cfg;
	float_t sampling_time;
	float_t tau;
	float_t lpf_constant;
	float_t filtered_value;
};

void lpf_first_order_init(lpf_first_order_t *const instance, lpf_first_order_cfg_t* cfg);

void lpf_first_order_process(lpf_first_order_t *const instance, float_t unfiltered_value);

float_t lpf_first_order_get_filtered_value(lpf_first_order_t *const instance);

#endif /* LPF_FIRST_ORDER_H_ */
