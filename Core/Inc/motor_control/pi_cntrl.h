#ifndef PI_CNTLR_H
#define	PI_CNTLR_H

typedef struct pi_cntrl_cfg pi_cntrl_cfg_t;
typedef struct pi_cntrl pi_cntrl_t;

struct pi_cntrl_cfg {
	float kp;
	float ki;
	float ts;
	float out_limit;
};

struct pi_cntrl {
    float kp;
    float ki_ts;
    float kt;
    float out_max;
    float out_min;
    float integral;
    float output;
};

void pi_cntrl_init(pi_cntrl_t* const instance, const pi_cntrl_cfg_t *cfg);

void pi_cntrl_reset(pi_cntrl_t* const instance);

float pi_cntrl_process(pi_cntrl_t* const instance, float error);

#endif /* PI_CNTLR_H */
