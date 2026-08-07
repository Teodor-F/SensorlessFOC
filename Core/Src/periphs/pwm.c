#include <pwm.h>
#include <stdbool.h>
#include <assert.h>

#define PWM_DEAD_TIME_NS	1000u
#define GIGAHERTZ_IN_HZ		1000000000.0f

typedef struct pwm pwm_t;

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
	pwm_phase_state_t phase_states;
};

static pwm_t pwm_object = {0};


static inline void timer_start(TIM_HandleTypeDef *tim)
{
    __HAL_TIM_ENABLE(tim);
}

static inline void timer_stop(TIM_HandleTypeDef *tim)
{
    __HAL_TIM_DISABLE(tim);
}

static inline void timer_reset(TIM_HandleTypeDef *tim)
{
    __HAL_TIM_SET_COUNTER(tim, 0u);
}


static inline void timer_complementary_pwm_channel_enable(TIM_HandleTypeDef *tim, uint32_t Channel, bool enable)
{
    uint32_t mask = (TIM_CCER_CC1E  << (Channel & 0x1FU)) | (TIM_CCER_CC1NE << (Channel & 0x0FU));
    tim->Instance->CCER = ( tim->Instance->CCER & ~mask) | (enable ? mask : 0U);
}


static inline void timer_master_channel_enable(TIM_HandleTypeDef *tim, bool enable)
{
    if (enable == true)
    {
        __HAL_TIM_MOE_ENABLE(tim);
    }
    else
    {
        __HAL_TIM_MOE_DISABLE_UNCONDITIONALLY(tim);
    }
}

void pwm_init(void)
{
	PWM_TIM_CLK_ENABLE();
	PWM_GPIO_PIN_CLK_ENABLE();

	pwm_object.periph_freq		= 	SYS_CLK_HZ;
	pwm_object.pwm_freq 		= 	PWM_FREQ_HZ;

	pwm_object.pwm_period 		= 	roundf(pwm_object.periph_freq / (2u * PWM_FREQ_HZ));
	pwm_object.half_pwm_period 	=	(pwm_object.pwm_period / 2u);

	float tick_resolution = (1.0f / pwm_object.periph_freq);
	float dead_time_ticks_f = (float)((PWM_DEAD_TIME_NS * 1e-9f) / tick_resolution);
	uint32_t dead_time_ticks = (uint32_t)(dead_time_ticks_f + 0.5f);

	// max PWM limit
	pwm_object.max_pwm_dc_scaled = pwm_object.pwm_period - dead_time_ticks;
	float duty_cycle_ratio = (float)pwm_object.max_pwm_dc_scaled / (float)pwm_object.pwm_period;
	pwm_object.max_pwm_dc = (uint32_t)(duty_cycle_ratio * PWM_DC_100 + 0.5f);


	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
	pwm_object.tim_periph.Instance = PWM_TIM;
	pwm_object.tim_periph.Init.Prescaler = 0u;
	pwm_object.tim_periph.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
	pwm_object.tim_periph.Init.Period = pwm_object.pwm_period;
	pwm_object.tim_periph.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	pwm_object.tim_periph.Init.RepetitionCounter = 0;
	pwm_object.tim_periph.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	assert(HAL_TIM_Base_Init(&pwm_object.tim_periph) == HAL_OK);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	assert(HAL_TIM_ConfigClockSource(&pwm_object.tim_periph, &sClockSourceConfig) == HAL_OK);
	assert(HAL_TIM_PWM_Init(&pwm_object.tim_periph) == HAL_OK);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_OC4REF;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	assert(HAL_TIMEx_MasterConfigSynchronization(&pwm_object.tim_periph, &sMasterConfig) == HAL_OK);


	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0u;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	assert(HAL_TIM_PWM_ConfigChannel(&pwm_object.tim_periph, &sConfigOC, TIM_CHANNEL_1) == HAL_OK);
	assert(HAL_TIM_PWM_ConfigChannel(&pwm_object.tim_periph, &sConfigOC, TIM_CHANNEL_2) == HAL_OK);
	assert(HAL_TIM_PWM_ConfigChannel(&pwm_object.tim_periph, &sConfigOC, TIM_CHANNEL_3) == HAL_OK);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = (pwm_object.pwm_period - 1u);
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	__HAL_TIM_DISABLE_OCxPRELOAD(&pwm_object.tim_periph, TIM_CHANNEL_4);
	assert(HAL_TIM_PWM_ConfigChannel(&pwm_object.tim_periph, &sConfigOC, TIM_CHANNEL_4) == HAL_OK);

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSR_ENABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 149u;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;

	assert(HAL_TIMEx_ConfigBreakDeadTime(&pwm_object.tim_periph, &sBreakDeadTimeConfig) == HAL_OK);

	GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PWM_UH_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = PWM_GPIO_PIN_AF;
    HAL_GPIO_Init(PWM_UH_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PWM_VH_GPIO_PIN;
    HAL_GPIO_Init(PWM_VH_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PWM_WH_GPIO_PIN;
    HAL_GPIO_Init(PWM_WH_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Pin = PWM_UL_GPIO_PIN;
    HAL_GPIO_Init(PWM_UL_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PWM_VL_GPIO_PIN;
    HAL_GPIO_Init(PWM_VL_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PWM_WL_GPIO_PIN;
    HAL_GPIO_Init(PWM_WL_GPIO_PORT, &GPIO_InitStruct);

//=======================================================================
    //Debug pin for ADC-triggering
    GPIO_InitStruct.Pin = PWM_ADC_TRIG_DBG_GPIO_PIN;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = PWM_ADC_TRIG_DBG_AF;
    HAL_GPIO_Init(PWM_ADC_TRIG_DBG_GPIO_PORT, &GPIO_InitStruct);

    pwm_object.duty_cycles.dc_phase_u = PWM_DC_50;
    pwm_object.duty_cycles.dc_phase_v = PWM_DC_50;
    pwm_object.duty_cycles.dc_phase_w = PWM_DC_50;
    pwm_set_duty_cycles( pwm_object.duty_cycles.dc_phase_u, pwm_object.duty_cycles.dc_phase_v, pwm_object.duty_cycles.dc_phase_w);

    // Enable ADC triggering
	SET_BIT(pwm_object.tim_periph.Instance->CCER, TIM_CCER_CC4E);
}

void pwm_start(void)
{
	timer_reset(&pwm_object.tim_periph);
	timer_complementary_pwm_channel_enable(&pwm_object.tim_periph, TIM_CHANNEL_1, true);
	timer_complementary_pwm_channel_enable(&pwm_object.tim_periph, TIM_CHANNEL_2, true);
	timer_complementary_pwm_channel_enable(&pwm_object.tim_periph, TIM_CHANNEL_3, true);
    timer_master_channel_enable(&pwm_object.tim_periph, true);
	timer_start(&pwm_object.tim_periph);
}

void pwm_stop(void)
{
	timer_master_channel_enable(&pwm_object.tim_periph, false);
	timer_complementary_pwm_channel_enable(&pwm_object.tim_periph, TIM_CHANNEL_1, false);
	timer_complementary_pwm_channel_enable(&pwm_object.tim_periph, TIM_CHANNEL_2, false);
	timer_complementary_pwm_channel_enable(&pwm_object.tim_periph, TIM_CHANNEL_3, false);
	timer_stop(&pwm_object.tim_periph);
}

void pwm_set_duty_cycles(const uint32_t dc_phase_u, const uint32_t dc_phase_v, const uint32_t dc_phase_w)
{

	uint32_t duty_cycle;
	uint32_t max_pwm_dc_scaled = pwm_object.max_pwm_dc_scaled;
	uint32_t max_pwm_dc = pwm_object.max_pwm_dc;

	if(dc_phase_u < max_pwm_dc)
	{
		duty_cycle = (pwm_object.max_pwm_dc_scaled * dc_phase_u) / max_pwm_dc;
		pwm_object.duty_cycles.dc_phase_u = duty_cycle;
	}
	else
	{
		pwm_object.duty_cycles.dc_phase_u = max_pwm_dc_scaled;
	}

	if (dc_phase_v < max_pwm_dc)
	{
		duty_cycle = (pwm_object.max_pwm_dc_scaled * dc_phase_v) / max_pwm_dc;
		pwm_object.duty_cycles.dc_phase_v = duty_cycle;
	}
	else
	{
		pwm_object.duty_cycles.dc_phase_v = max_pwm_dc_scaled;
	}

	if (dc_phase_w < max_pwm_dc)
	{
		duty_cycle = (pwm_object.max_pwm_dc_scaled * dc_phase_w) / max_pwm_dc;
		pwm_object.duty_cycles.dc_phase_w = duty_cycle;
	}
	else
	{
		pwm_object.duty_cycles.dc_phase_w = max_pwm_dc_scaled;
	}

    pwm_object.tim_periph.Instance->CCR1 = (uint32_t)(pwm_object.duty_cycles.dc_phase_u);
    pwm_object.tim_periph.Instance->CCR2 = (uint32_t)(pwm_object.duty_cycles.dc_phase_v);
    pwm_object.tim_periph.Instance->CCR3 = (uint32_t)(pwm_object.duty_cycles.dc_phase_w);
}

void pwm_force_off(void)
{
	timer_master_channel_enable(&pwm_object.tim_periph, false);
	timer_stop(&pwm_object.tim_periph);
	pwm_set_duty_cycles(PWM_DC_50, PWM_DC_50, PWM_DC_50);
}


pwm_duty_cycles_t pwm_get_duty_cycles(void)
{
	pwm_duty_cycles_t dc;
	dc.dc_phase_u = pwm_object.duty_cycles.dc_phase_u;
	dc.dc_phase_v = pwm_object.duty_cycles.dc_phase_v;
	dc.dc_phase_w = pwm_object.duty_cycles.dc_phase_w;
	return dc;
}
