#include <adc.h>

#include <stdbool.h>
#include <assert.h>

#define ADC_CURR_A_IDX		0u
#define ADC_CURR_B_IDX		1u
#define ADC_CURR_C_IDX		2u

static adc_t *p_adc = NULL;

void ADC1_2_IRQHandler(void)
{
	HAL_ADC_IRQHandler(&p_adc->adc_periph_1);
	HAL_ADC_IRQHandler(&p_adc->adc_periph_2);
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	UNUSED(hadc);
}

void adc_init(adc_t *const instance)
{
	ADC_CLK_ENABLE();

	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
	PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
	assert(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) == HAL_OK);

	ADC_InjectionConfTypeDef sconfigInjected1 = {0};
	ADC_InjectionConfTypeDef sconfigInjected2 = {0};
	ADC_MultiModeTypeDef multimode = {0};

//========================================================================
	//ADC1
	instance->adc_periph_1.Instance = ADC1;
	instance->adc_periph_1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
	instance->adc_periph_1.Init.Resolution = ADC_RESOLUTION_12B;
	instance->adc_periph_1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	instance->adc_periph_1.Init.GainCompensation = 0;
	instance->adc_periph_1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	instance->adc_periph_1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	instance->adc_periph_1.Init.LowPowerAutoWait = DISABLE;
	instance->adc_periph_1.Init.ContinuousConvMode = DISABLE;
	instance->adc_periph_1.Init.NbrOfConversion = 1;
	instance->adc_periph_1.Init.DiscontinuousConvMode = DISABLE;
	instance->adc_periph_1.Init.DMAContinuousRequests = DISABLE;
	instance->adc_periph_1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	instance->adc_periph_1.Init.OversamplingMode = DISABLE;

	assert(HAL_ADC_Init(&instance->adc_periph_1) == HAL_OK);

	multimode.Mode = ADC_MODE_INDEPENDENT;
	assert(HAL_ADCEx_MultiModeConfigChannel(&instance->adc_periph_1, &multimode) == HAL_OK);

//========================================================================
	//PHASE-A current
	sconfigInjected1.InjectedChannel = ADC_CURR_A_CHANNEL;
	sconfigInjected1.InjectedRank = ADC_INJECTED_RANK_1;
	sconfigInjected1.InjectedSamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	sconfigInjected1.InjectedSingleDiff = ADC_SINGLE_ENDED;
	sconfigInjected1.InjectedOffsetNumber = ADC_OFFSET_NONE;
	sconfigInjected1.InjectedOffset = 0;
	sconfigInjected1.InjectedNbrOfConversion = 2;
	sconfigInjected1.InjectedDiscontinuousConvMode = DISABLE;
	sconfigInjected1.AutoInjectedConv = DISABLE;
	sconfigInjected1.QueueInjectedContext = DISABLE;
	sconfigInjected1.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJEC_T1_TRGO2;
	sconfigInjected1.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_RISING;
	sconfigInjected1.InjecOversamplingMode = DISABLE;
	assert(HAL_ADCEx_InjectedConfigChannel(&instance->adc_periph_1, &sconfigInjected1) == HAL_OK);
//======================================================================
	//PHASE-B current
	sconfigInjected1.InjectedSamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	sconfigInjected1.InjectedChannel = ADC_CURR_B_CHANNEL;
	sconfigInjected1.InjectedRank = ADC_INJECTED_RANK_2;
	assert(HAL_ADCEx_InjectedConfigChannel(&instance->adc_periph_1, &sconfigInjected1) == HAL_OK);

//========================================================================
	//ADC2
	instance->adc_periph_2.Instance = ADC2;
	instance->adc_periph_2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
	instance->adc_periph_2.Init.Resolution = ADC_RESOLUTION_12B;
	instance->adc_periph_2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	instance->adc_periph_2.Init.GainCompensation = 0;
	instance->adc_periph_2.Init.ScanConvMode = ADC_SCAN_DISABLE;
	instance->adc_periph_2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	instance->adc_periph_2.Init.LowPowerAutoWait = DISABLE;
	instance->adc_periph_2.Init.ContinuousConvMode = DISABLE;
	instance->adc_periph_2.Init.NbrOfConversion = 1;
	instance->adc_periph_2.Init.DiscontinuousConvMode = DISABLE;
	instance->adc_periph_2.Init.DMAContinuousRequests = DISABLE;
	instance->adc_periph_2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	instance->adc_periph_2.Init.OversamplingMode = DISABLE;
	assert(HAL_ADC_Init(&instance->adc_periph_2) == HAL_OK);

//=======================================================================
	//PHASE-C current
	sconfigInjected2.InjectedChannel = ADC_CURR_C_CHANNEL;
	sconfigInjected2.InjectedRank = ADC_INJECTED_RANK_1;
	sconfigInjected2.InjectedSamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	sconfigInjected2.InjectedSingleDiff = ADC_SINGLE_ENDED;
	sconfigInjected2.InjectedOffsetNumber = ADC_OFFSET_NONE;
	sconfigInjected2.InjectedOffset = 0;
	sconfigInjected2.InjectedNbrOfConversion = 1;
	sconfigInjected2.InjectedDiscontinuousConvMode = DISABLE;
	sconfigInjected2.AutoInjectedConv = DISABLE;
	sconfigInjected2.QueueInjectedContext = DISABLE;
	sconfigInjected2.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJEC_T1_TRGO2;
	sconfigInjected2.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_RISING;
	sconfigInjected2.InjecOversamplingMode = DISABLE;
	assert(HAL_ADCEx_InjectedConfigChannel(&instance->adc_periph_2, &sconfigInjected2) == HAL_OK);

//=======================================================================
	//GPIO PHASE-A, PHASE-B, PHASE-C
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	GPIO_InitStruct.Pin = ADC_CURR_A_GPIO_PIN;
	HAL_GPIO_Init(ADC_CURR_A_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = ADC_CURR_B_GPIO_PIN;
	HAL_GPIO_Init(ADC_CURR_B_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = ADC_CURR_C_GPIO_PIN;
	HAL_GPIO_Init(ADC_CURR_C_GPIO_PORT, &GPIO_InitStruct);

	instance->adc_currents[ADC_CURR_A_IDX] = (uint16_t*)&(instance->adc_periph_1.Instance->JDR1);
	instance->adc_currents[ADC_CURR_B_IDX] = (uint16_t*)&(instance->adc_periph_1.Instance->JDR2);
	instance->adc_currents[ADC_CURR_C_IDX] = (uint16_t*)&(instance->adc_periph_2.Instance->JDR1);

	HAL_ADCEx_Calibration_Start(&instance->adc_periph_1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&instance->adc_periph_2, ADC_SINGLE_ENDED);

    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
    p_adc = instance;

	assert(HAL_ADCEx_InjectedStart_IT(&instance->adc_periph_1) == HAL_OK && HAL_ADCEx_InjectedStart_IT(&instance->adc_periph_2) == HAL_OK);
}

adc_currents_t adc_get_currents(adc_t *const instance)
{
	adc_currents_t retVal;
	retVal.curr_a = *(instance->adc_currents[ADC_CURR_A_IDX]);
	retVal.curr_b = *(instance->adc_currents[ADC_CURR_B_IDX]);
	retVal.curr_c = *(instance->adc_currents[ADC_CURR_C_IDX]);
	return retVal;
}

uint16_t adc_get_signal(adc_t *const instance, adc_signal_t adc_signal)
{
	UNUSED(instance);
	UNUSED(adc_signal);
	return 0;
}


