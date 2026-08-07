#ifndef MC_TIMER_H
#define MC_TIMER_H

#include "mc_callback.h"

typedef enum mc_timer_callback_idx  mc_timer_callback_idx_t;

enum mc_timer_callback_idx
{
	MCTIMER_CB_IDX_1,
	MCTIMER_CB_IDX_2,
};

void mc_timer_init(void);

void mc_timer_start(void);

void mc_timer_stop(void);

void mc_timer_reset(void);

void mc_timer_register_mc_callback(mc_timer_callback_idx_t cb_idx, mc_callback_function_t function, mc_callback_param_t param);

void mc_timer_activate_callback(mc_timer_callback_idx_t cb_idx);

void mc_timer_deactivate_callback(mc_timer_callback_idx_t cb_idx);

#endif /* MC_TIMER_H */
