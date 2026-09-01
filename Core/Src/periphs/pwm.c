#include <pwm.h>
#include <stdbool.h>
#include <assert.h>

#define PWM_DEAD_TIME_NS	1000u
#define GIGAHERTZ_IN_HZ		1000000000.0f

static pwm_t *p_pwm = NULL;

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

void pwm_init(pwm_t *const instance)
{
	PWM_TIM_CLK_ENABLE();
	PWM_GPIO_PIN_CLK_ENABLE();

	instance->periph_freq = SYS_CLK_HZ;
	instance->pwm_freq = PWM_FREQ_HZ;

	instance->pwm_period = roundf(instance->periph_freq / (2u * PWM_FREQ_HZ));
	instance->half_pwm_period =	(instance->pwm_period / 2u);

	float_t tick_resolution = (1.0f / instance->periph_freq);
	float_t dead_time_ticks_f = (float_t)((PWM_DEAD_TIME_NS * 1e-9f) / tick_resolution);
	uint32_t dead_time_ticks = (uint32_t)(dead_time_ticks_f + 0.5f);

	// max PWM limit
	instance->max_pwm_dc_scaled = instance->pwm_period - dead_time_ticks;
	float_t duty_cycle_ratio = (float_t)instance->max_pwm_dc_scaled / (float_t)instance->pwm_period;
	instance->max_pwm_dc = (uint32_t)(duty_cycle_ratio * PWM_DC_100 + 0.5f);


	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
	instance->tim_periph.Instance = PWM_TIM;
	instance->tim_periph.Init.Prescaler = 0u;
	instance->tim_periph.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
	instance->tim_periph.Init.Period = instance->pwm_period;
	instance->tim_periph.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	instance->tim_periph.Init.RepetitionCounter = 0;
	instance->tim_periph.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	assert(HAL_TIM_Base_Init(&instance->tim_periph) == HAL_OK);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	assert(HAL_TIM_ConfigClockSource(&instance->tim_periph, &sClockSourceConfig) == HAL_OK);
	assert(HAL_TIM_PWM_Init(&instance->tim_periph) == HAL_OK);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_OC4REF;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	assert(HAL_TIMEx_MasterConfigSynchronization(&instance->tim_periph, &sMasterConfig) == HAL_OK);


	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0u;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	assert(HAL_TIM_PWM_ConfigChannel(&instance->tim_periph, &sConfigOC, TIM_CHANNEL_1) == HAL_OK);
	assert(HAL_TIM_PWM_ConfigChannel(&instance->tim_periph, &sConfigOC, TIM_CHANNEL_2) == HAL_OK);
	assert(HAL_TIM_PWM_ConfigChannel(&instance->tim_periph, &sConfigOC, TIM_CHANNEL_3) == HAL_OK);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = (instance->pwm_period - 1u);
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	__HAL_TIM_DISABLE_OCxPRELOAD(&instance->tim_periph, TIM_CHANNEL_4);
	assert(HAL_TIM_PWM_ConfigChannel(&instance->tim_periph, &sConfigOC, TIM_CHANNEL_4) == HAL_OK);

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

	assert(HAL_TIMEx_ConfigBreakDeadTime(&instance->tim_periph, &sBreakDeadTimeConfig) == HAL_OK);

	GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PWM_PHASE_AH_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = PWM_GPIO_PIN_AF;
    HAL_GPIO_Init(PWM_PHASE_AH_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PWM_PHASE_BH_GPIO_PIN;
    HAL_GPIO_Init(PWM_PHASE_BH_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PWM_PHASE_CH_GPIO_PIN;
    HAL_GPIO_Init(PWM_PHASE_CH_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Pin = PWM_PHASE_AL_GPIO_PIN;
    HAL_GPIO_Init(PWM_PHASE_AL_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PWM_PHASE_BL_GPIO_PIN;
    HAL_GPIO_Init(PWM_PHASE_BL_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PWM_PHASE_CL_GPIO_PIN;
    HAL_GPIO_Init(PWM_PHASE_CL_GPIO_PORT, &GPIO_InitStruct);

//=======================================================================
    //Debug pin for ADC-triggering
    GPIO_InitStruct.Pin = PWM_ADC_TRIG_DBG_GPIO_PIN;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = PWM_ADC_TRIG_DBG_AF;
    HAL_GPIO_Init(PWM_ADC_TRIG_DBG_GPIO_PORT, &GPIO_InitStruct);

    instance->duty_cycles.dc_phase_u = PWM_DC_50;
    instance->duty_cycles.dc_phase_v = PWM_DC_50;
    instance->duty_cycles.dc_phase_w = PWM_DC_50;
    pwm_set_duty_cycles(instance, instance->duty_cycles.dc_phase_u, instance->duty_cycles.dc_phase_v, instance->duty_cycles.dc_phase_w);

    // Enable ADC triggering
	SET_BIT(instance->tim_periph.Instance->CCER, TIM_CCER_CC4E);

	p_pwm = instance;
}

void pwm_start(pwm_t *const instance)
{
	timer_reset(&instance->tim_periph);
	timer_complementary_pwm_channel_enable(&instance->tim_periph, TIM_CHANNEL_1, true);
	timer_complementary_pwm_channel_enable(&instance->tim_periph, TIM_CHANNEL_2, true);
	timer_complementary_pwm_channel_enable(&instance->tim_periph, TIM_CHANNEL_3, true);
    timer_master_channel_enable(&instance->tim_periph, true);
	timer_start(&instance->tim_periph);
}

void pwm_stop(pwm_t *const instance)
{
	timer_master_channel_enable(&instance->tim_periph, false);
	timer_complementary_pwm_channel_enable(&instance->tim_periph, TIM_CHANNEL_1, false);
	timer_complementary_pwm_channel_enable(&instance->tim_periph, TIM_CHANNEL_2, false);
	timer_complementary_pwm_channel_enable(&instance->tim_periph, TIM_CHANNEL_3, false);
	timer_stop(&instance->tim_periph);
}

void pwm_set_duty_cycles(pwm_t *const instance, const uint32_t dc_phase_u, const uint32_t dc_phase_v, const uint32_t dc_phase_w)
{

	uint32_t duty_cycle;
	uint32_t max_pwm_dc_scaled = instance->max_pwm_dc_scaled;
	uint32_t max_pwm_dc = instance->max_pwm_dc;

	if(dc_phase_u < max_pwm_dc)
	{
		duty_cycle = (instance->max_pwm_dc_scaled * dc_phase_u) / max_pwm_dc;
		instance->duty_cycles.dc_phase_u = duty_cycle;
	}
	else
	{
		instance->duty_cycles.dc_phase_u = max_pwm_dc_scaled;
	}

	if (dc_phase_v < max_pwm_dc)
	{
		duty_cycle = (instance->max_pwm_dc_scaled * dc_phase_v) / max_pwm_dc;
		instance->duty_cycles.dc_phase_v = duty_cycle;
	}
	else
	{
		instance->duty_cycles.dc_phase_v = max_pwm_dc_scaled;
	}

	if (dc_phase_w < max_pwm_dc)
	{
		duty_cycle = (instance->max_pwm_dc_scaled * dc_phase_w) / max_pwm_dc;
		instance->duty_cycles.dc_phase_w = duty_cycle;
	}
	else
	{
		instance->duty_cycles.dc_phase_w = max_pwm_dc_scaled;
	}

    instance->tim_periph.Instance->CCR1 = (uint32_t)(instance->duty_cycles.dc_phase_u);
    instance->tim_periph.Instance->CCR2 = (uint32_t)(instance->duty_cycles.dc_phase_v);
    instance->tim_periph.Instance->CCR3 = (uint32_t)(instance->duty_cycles.dc_phase_w);
}

void pwm_force_off(pwm_t *const instance)
{
	timer_master_channel_enable(&instance->tim_periph, false);
	timer_stop(&instance->tim_periph);
	pwm_set_duty_cycles(instance, PWM_DC_50, PWM_DC_50, PWM_DC_50);
}


pwm_duty_cycles_t pwm_get_duty_cycles(pwm_t *const instance)
{
	pwm_duty_cycles_t dc;
	dc.dc_phase_u = instance->duty_cycles.dc_phase_u;
	dc.dc_phase_v = instance->duty_cycles.dc_phase_v;
	dc.dc_phase_w = instance->duty_cycles.dc_phase_w;
	return dc;
}
