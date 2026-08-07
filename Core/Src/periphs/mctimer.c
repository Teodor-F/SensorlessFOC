#include <mctimer.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

typedef struct mc_timer mc_timer_t;

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

static mc_timer_t mc_timer_object = {0u};



void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(mc_timer_object.tim_periph.Instance == htim->Instance)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1 && mc_timer_object.first_callback_active)
		{
			HAL_GPIO_WritePin(MC_TIM_EVENT1_DBG_PORT, MC_TIM_EVENT1_DBG_PIN, GPIO_PIN_SET);
			HAL_GPIO_WritePin(MC_TIM_EVENT1_DBG_PORT, MC_TIM_EVENT1_DBG_PIN, GPIO_PIN_RESET);
			mc_callback_execute(&(mc_timer_object.first_callback));
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2 && mc_timer_object.second_callback_active)
		{
			HAL_GPIO_WritePin(MC_TIM_EVENT1_DBG_PORT, MC_TIM_EVENT1_DBG_PIN, GPIO_PIN_SET);
			HAL_GPIO_WritePin(MC_TIM_EVENT1_DBG_PORT, MC_TIM_EVENT1_DBG_PIN, GPIO_PIN_RESET);
			mc_callback_execute(&(mc_timer_object.second_callback));
		}
	}

}

void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&(mc_timer_object.tim_periph));
}

void mc_timer_init(void)
{

	MC_TIM_CLK_ENABLE();
	mc_timer_object.periph_freq = SYS_CLK_HZ;
	mc_timer_object.timer_freq = PWM_FREQ_HZ;

	mc_timer_object.half_period_value = roundf(mc_timer_object.periph_freq / (2u * PWM_FREQ_HZ));
	mc_timer_object.period_value = 2 * mc_timer_object.half_period_value - 1;

	mc_timer_object.first_callback_trig_pt = (uint32_t)((float)mc_timer_object.period_value * 0.25f);
	mc_timer_object.second_callback_trig_pt = (uint32_t)((float)mc_timer_object.period_value * 0.75f);


	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	mc_timer_object.tim_periph.Instance = MC_TIM;
	mc_timer_object.tim_periph.Init.Prescaler = 0;
	mc_timer_object.tim_periph.Init.CounterMode = TIM_COUNTERMODE_UP;
	mc_timer_object.tim_periph.Init.Period = mc_timer_object.period_value;
	mc_timer_object.tim_periph.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	mc_timer_object.tim_periph.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&mc_timer_object.tim_periph) != HAL_OK)
	{
		assert(0);
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&mc_timer_object.tim_periph, &sClockSourceConfig) != HAL_OK)
	{
		assert(0);
	}
	if (HAL_TIM_OC_Init(&mc_timer_object.tim_periph) != HAL_OK)
	{
		assert(0);
	}

	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
	sSlaveConfig.InputTrigger = TIM_TS_ITR0;
	if (HAL_TIM_SlaveConfigSynchro(&mc_timer_object.tim_periph, &sSlaveConfig) != HAL_OK)
	{
		assert(0);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&mc_timer_object.tim_periph, &sMasterConfig) != HAL_OK)
	{
		assert(0);
	}
	sConfigOC.OCMode = TIM_OCMODE_TIMING;
	sConfigOC.Pulse = mc_timer_object.first_callback_trig_pt;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&mc_timer_object.tim_periph, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		assert(0);
	}
	__HAL_TIM_ENABLE_OCxPRELOAD(&mc_timer_object.tim_periph, TIM_CHANNEL_1);


	sConfigOC.Pulse = mc_timer_object.second_callback_trig_pt;
	if (HAL_TIM_OC_ConfigChannel(&mc_timer_object.tim_periph, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		assert(0);
	}
	__HAL_TIM_ENABLE_OCxPRELOAD(&mc_timer_object.tim_periph, TIM_CHANNEL_2);

	mc_timer_object.first_callback_active = false;
	mc_timer_object.second_callback_active = false;

	mc_callback_init(&(mc_timer_object.first_callback));
	mc_callback_init(&(mc_timer_object.second_callback));

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

    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void mc_timer_start(void)
{
	HAL_TIM_Base_Start_IT(&(mc_timer_object.tim_periph));
	HAL_TIM_OC_Start_IT(&(mc_timer_object.tim_periph), TIM_CHANNEL_1);
	HAL_TIM_OC_Start_IT(&(mc_timer_object.tim_periph), TIM_CHANNEL_2);
}

void mc_timer_stop(void)
{
	HAL_TIM_Base_Stop_IT(&(mc_timer_object.tim_periph));
	HAL_TIM_OC_Stop_IT(&(mc_timer_object.tim_periph), TIM_CHANNEL_1);
	HAL_TIM_OC_Stop_IT(&(mc_timer_object.tim_periph), TIM_CHANNEL_2);
}

void mc_timer_reset(void)
{
	mc_timer_stop();

	mc_timer_object.first_callback_trig_pt = mc_timer_object.period_value;
	mc_timer_object.second_callback_trig_pt = mc_timer_object.period_value;
	__HAL_TIM_SET_COUNTER(&(mc_timer_object.tim_periph), 0u);
	__HAL_TIM_SET_COMPARE(&(mc_timer_object.tim_periph), TIM_CHANNEL_1, mc_timer_object.first_callback_trig_pt);
	__HAL_TIM_SET_COMPARE(&(mc_timer_object.tim_periph), TIM_CHANNEL_2, mc_timer_object.second_callback_trig_pt);
}


void mc_timer_register_mc_callback(mc_timer_callback_idx_t cb_idx, mc_callback_function_t function, mc_callback_param_t param)
{
	if(cb_idx == MCTIMER_CB_IDX_1)
	{
		mc_callback_register_function(&(mc_timer_object.first_callback), function, param);
	}
	else if(cb_idx == MCTIMER_CB_IDX_2)
	{
		mc_callback_register_function(&(mc_timer_object.second_callback), function, param);
	}
	else
	{
		assert(0);
	}
}

void mc_timer_activate_callback(mc_timer_callback_idx_t cb_idx)
{
   if (cb_idx == MCTIMER_CB_IDX_1)
   {
	   __disable_irq();
	   mc_timer_object.first_callback_active = true;
	   __enable_irq();
   }
   else if(cb_idx == MCTIMER_CB_IDX_2)
   {
	   __disable_irq();
	   mc_timer_object.second_callback_active = true;
	   __enable_irq();
   }
   else
   {
	   assert(0);
   }
}

void mc_timer_deactivate_callback(mc_timer_callback_idx_t cb_idx)
{
	uint32_t timerChannel = TIM_CHANNEL_1;
	uint32_t timerChannelFlag = TIM_FLAG_CC1;

	if (cb_idx == MCTIMER_CB_IDX_1)
	{
		 __disable_irq();
		mc_timer_object.first_callback_active = false;
		__enable_irq();
	}
	else if (cb_idx == MCTIMER_CB_IDX_2)
	{
		__disable_irq();
		timerChannel = TIM_CHANNEL_2;
		timerChannelFlag = TIM_FLAG_CC2;
		mc_timer_object.second_callback_active = false;
		__enable_irq();

	}
	else
	{
		assert(0);
	}

	HAL_TIM_OC_Stop_IT(&(mc_timer_object.tim_periph), timerChannel);
	__HAL_TIM_CLEAR_FLAG(&(mc_timer_object.tim_periph), timerChannelFlag);
}

