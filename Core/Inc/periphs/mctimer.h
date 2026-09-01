#ifndef MC_TIMER_H
#define MC_TIMER_H

#include <mc_callback.h>

typedef enum mc_timer_callback_idx  mc_timer_callback_idx_t;
typedef struct mc_timer mc_timer_t;

enum mc_timer_callback_idx
{
	MCTIMER_CB_IDX_1,
	MCTIMER_CB_IDX_2,
};

struct mc_timer
{
	TIM_HandleTypeDef tim_periph;
	uint32_t periph_freq;
	uint32_t timer_freq;
	uint32_t period_value;
	uint32_t half_period_value;
	mc_callback_t first_callback;
	mc_callback_t second_callback;

	uint32_t first_callback_trig_pt;
	uint32_t second_callback_trig_pt;
	volatile bool first_callback_active;
	volatile bool second_callback_active;
};


void mc_timer_init(mc_timer_t *const instance);

void mc_timer_start(mc_timer_t *const instance);

void mc_timer_stop(mc_timer_t *const instance);

void mc_timer_reset(mc_timer_t *const instance);

void mc_timer_register_mc_callback(mc_timer_t *const instance, mc_timer_callback_idx_t cb_idx, mc_callback_function_t function, mc_callback_param_t param);

void mc_timer_activate_callback(mc_timer_t *const instance, mc_timer_callback_idx_t cb_idx);

void mc_timer_deactivate_callback(mc_timer_t *const instance, mc_timer_callback_idx_t cb_idx);

#endif /* MC_TIMER_H */
