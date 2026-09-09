#include <mctimer.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>


static mc_timer_t *p_mc_timer = NULL;

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(p_mc_timer->tim_periph.Instance == htim->Instance)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1 && p_mc_timer->first_callback_active)
		{
			HAL_GPIO_WritePin(MC_TIM_EVENT1_DBG_PORT, MC_TIM_EVENT1_DBG_PIN, GPIO_PIN_SET);
			HAL_GPIO_WritePin(MC_TIM_EVENT1_DBG_PORT, MC_TIM_EVENT1_DBG_PIN, GPIO_PIN_RESET);
			mc_callback_execute(&(p_mc_timer->first_callback));
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2 && p_mc_timer->second_callback_active)
		{
			HAL_GPIO_WritePin(MC_TIM_EVENT1_DBG_PORT, MC_TIM_EVENT1_DBG_PIN, GPIO_PIN_SET);
			HAL_GPIO_WritePin(MC_TIM_EVENT1_DBG_PORT, MC_TIM_EVENT1_DBG_PIN, GPIO_PIN_RESET);
			mc_callback_execute(&(p_mc_timer->second_callback));
		}
	}

}

void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&(p_mc_timer->tim_periph));
}

void mc_timer_init(mc_timer_t *const instance)
{

	MC_TIM_CLK_ENABLE();
	instance->periph_freq = SYS_CLK_HZ;
	instance->timer_freq = PWM_FREQ_HZ;

	instance->half_period_value = roundf(instance->periph_freq / (2u * PWM_FREQ_HZ));
	instance->period_value = 2 * instance->half_period_value - 1;

	instance->first_callback_trig_pt = (uint32_t)((float)instance->period_value * 0.25f);
	instance->second_callback_trig_pt = (uint32_t)((float)instance->period_value * 0.75f);


	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	instance->tim_periph.Instance = MC_TIM;
	instance->tim_periph.Init.Prescaler = 0;
	instance->tim_periph.Init.CounterMode = TIM_COUNTERMODE_UP;
	instance->tim_periph.Init.Period = instance->period_value;
	instance->tim_periph.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	instance->tim_periph.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&instance->tim_periph) != HAL_OK)
	{
		assert(0);
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&instance->tim_periph, &sClockSourceConfig) != HAL_OK)
	{
		assert(0);
	}
	if (HAL_TIM_OC_Init(&instance->tim_periph) != HAL_OK)
	{
		assert(0);
	}

	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
	sSlaveConfig.InputTrigger = TIM_TS_ITR0;
	if (HAL_TIM_SlaveConfigSynchro(&instance->tim_periph, &sSlaveConfig) != HAL_OK)
	{
		assert(0);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&instance->tim_periph, &sMasterConfig) != HAL_OK)
	{
		assert(0);
	}
	sConfigOC.OCMode = TIM_OCMODE_TIMING;
	sConfigOC.Pulse = instance->first_callback_trig_pt;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&instance->tim_periph, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		assert(0);
	}
	__HAL_TIM_ENABLE_OCxPRELOAD(&instance->tim_periph, TIM_CHANNEL_1);


	sConfigOC.Pulse = instance->second_callback_trig_pt;
	if (HAL_TIM_OC_ConfigChannel(&instance->tim_periph, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		assert(0);
	}
	__HAL_TIM_ENABLE_OCxPRELOAD(&instance->tim_periph, TIM_CHANNEL_2);

	instance->first_callback_active = false;
	instance->second_callback_active = false;

	mc_callback_init(&(instance->first_callback));
	mc_callback_init(&(instance->second_callback));

//=======================================================================
	//Debug pins for event triggering
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = MC_TIM_EVENT1_DBG_PIN;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(MC_TIM_EVENT1_DBG_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = MC_TIM_EVENT2_DBG_PIN;
	HAL_GPIO_Init(MC_TIM_EVENT2_DBG_PORT, &GPIO_InitStruct);

	p_mc_timer = instance;

    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

}

void mc_timer_start(mc_timer_t *const instance)
{
	HAL_TIM_Base_Start_IT(&(instance->tim_periph));
	HAL_TIM_OC_Start_IT(&(instance->tim_periph), TIM_CHANNEL_1);
	HAL_TIM_OC_Start_IT(&(instance->tim_periph), TIM_CHANNEL_2);
}

void mc_timer_stop(mc_timer_t *const instance)
{
	HAL_TIM_Base_Stop_IT(&(instance->tim_periph));
	HAL_TIM_OC_Stop_IT(&(instance->tim_periph), TIM_CHANNEL_1);
	HAL_TIM_OC_Stop_IT(&(instance->tim_periph), TIM_CHANNEL_2);
}

void mc_timer_reset(mc_timer_t *const instance)
{
	mc_timer_stop(instance);

	instance->first_callback_trig_pt = instance->period_value;
	instance->second_callback_trig_pt = instance->period_value;
	__HAL_TIM_SET_COUNTER(&(instance->tim_periph), 0u);
	__HAL_TIM_SET_COMPARE(&(instance->tim_periph), TIM_CHANNEL_1, instance->first_callback_trig_pt);
	__HAL_TIM_SET_COMPARE(&(instance->tim_periph), TIM_CHANNEL_2, instance->second_callback_trig_pt);
}


void mc_timer_register_mc_callback(mc_timer_t *const instance, mc_timer_callback_idx_t cb_idx, mc_callback_function_t function, mc_callback_param_t param)
{
	if(cb_idx == MCTIMER_CB_IDX_1)
	{
		mc_callback_register_function(&(instance->first_callback), function, param);
	}
	else if(cb_idx == MCTIMER_CB_IDX_2)
	{
		mc_callback_register_function(&(instance->second_callback), function, param);
	}
	else
	{
		assert(0);
	}
}

void mc_timer_activate_callback(mc_timer_t *const instance, mc_timer_callback_idx_t cb_idx)
{
   if (cb_idx == MCTIMER_CB_IDX_1)
   {
	   __disable_irq();
	   instance->first_callback_active = true;
	   __enable_irq();
   }
   else if(cb_idx == MCTIMER_CB_IDX_2)
   {
	   __disable_irq();
	   instance->second_callback_active = true;
	   __enable_irq();
   }
   else
   {
	   assert(0);
   }
}

void mc_timer_deactivate_callback(mc_timer_t *const instance, mc_timer_callback_idx_t cb_idx)
{
	uint32_t timerChannel = TIM_CHANNEL_1;
	uint32_t timerChannelFlag = TIM_FLAG_CC1;

	if (cb_idx == MCTIMER_CB_IDX_1)
	{
		 __disable_irq();
		instance->first_callback_active = false;
		__enable_irq();
	}
	else if (cb_idx == MCTIMER_CB_IDX_2)
	{
		__disable_irq();
		timerChannel = TIM_CHANNEL_2;
		timerChannelFlag = TIM_FLAG_CC2;
		instance->second_callback_active = false;
		__enable_irq();

	}
	else
	{
		assert(0);
	}

	HAL_TIM_OC_Stop_IT(&(instance->tim_periph), timerChannel);
	__HAL_TIM_CLEAR_FLAG(&(instance->tim_periph), timerChannelFlag);
}


uint32_t mc_timer_get_freq(mc_timer_t *const instance)
{
	uint32_t ret_val = 0u;
	ret_val = instance->timer_freq;
	return ret_val;
}
