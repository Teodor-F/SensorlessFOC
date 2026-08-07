#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <stdbool.h>

#define PWM_DC_100	(UINT16_MAX)
#define PWM_DC_50	(UINT16_MAX / 2u)

typedef struct pwm_duty_cycles pwm_duty_cycles_t;
typedef enum pwm_phase_state pwm_phase_state_t;

struct pwm_duty_cycles
{
    uint32_t	dc_phase_u;
    uint32_t    dc_phase_v;
    uint32_t    dc_phase_w;
};

enum pwm_phase_state
{
    PWM_UVW_OFF   = 0x00,
    PWM_W_ON      = 0x04,
	PWM_V_ON      = 0x02,
	PWM_VW_ON     = 0x06,
	PWM_U_ON      = 0x01,
	PWM_UW_ON     = 0x05,
	PWM_UV_ON     = 0x03,
	PWM_UVW_ON    = 0x07
};

void pwm_init(void);

void pwm_start(void);

void pwm_stop(void);

void pwm_set_duty_cycles(const uint32_t dc_phase_u, const uint32_t dc_phase_v, const uint32_t dc_phase_w);

void pwm_force_off(void);

pwm_duty_cycles_t pwm_get_duty_cycles(void);

#endif /* PWM_H */
