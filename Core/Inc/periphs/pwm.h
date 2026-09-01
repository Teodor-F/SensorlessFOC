#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <stdbool.h>

#define PWM_DC_100	((uint16_t)(UINT16_MAX))
#define PWM_DC_50	((uint16_t)(UINT16_MAX / 2u))

typedef struct pwm pwm_t;
typedef struct pwm_duty_cycles pwm_duty_cycles_t;
typedef enum pwm_phase_state pwm_phase_state_t;

struct pwm_duty_cycles
{
    uint32_t	dc_phase_u;
    uint32_t    dc_phase_v;
    uint32_t    dc_phase_w;
};

struct pwm
{
	TIM_HandleTypeDef tim_periph;
	uint32_t pwm_period;
	uint32_t half_pwm_period;
	uint32_t pwm_freq;
	uint32_t periph_freq;
	uint32_t max_pwm_dc_scaled;
	uint32_t max_pwm_dc;
	pwm_duty_cycles_t duty_cycles;
};

void pwm_init(pwm_t *const instance);

void pwm_start(pwm_t *const instance);

void pwm_stop(pwm_t *const instance);

void pwm_set_duty_cycles(pwm_t *const instance, const uint32_t dc_phase_u, const uint32_t dc_phase_v, const uint32_t dc_phase_w);

void pwm_force_off(pwm_t *const instance);

pwm_duty_cycles_t pwm_get_duty_cycles(pwm_t *const instance);

#endif /* PWM_H */
