#include <adc.h>
#include <stdbool.h>

#include <assert.h>
#include <config/motor_cfg.h>

#define ADC_CURRENTS_NUM 	3u
#define ADC_IU_IDX			0u
#define ADC_IV_IDX			1u
#define ADC_IW_IDX			2u

typedef struct adc adc_t;

struct adc {
	ADC_HandleTypeDef adc_periph_1;
	ADC_HandleTypeDef adc_periph_2;
	uint16_t *adc_currents[ADC_CURRENTS_NUM];
};

static adc_t adc_object  = {0};

void ADC1_2_IRQHandler(void);


void ADC1_2_IRQHandler(void)
{
	HAL_ADC_IRQHandler(&adc_object.adc_periph_1);
	HAL_ADC_IRQHandler(&adc_object.adc_periph_2);
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	static volatile bool adc_1_done = false;
	static volatile bool adc_2_done = false;

	if(adc_object.adc_periph_1.Instance == hadc->Instance)
	{
		adc_1_done = true;
	}
	else if (adc_object.adc_periph_2.Instance == hadc->Instance)
	{
		adc_2_done = true;
	}
}

void adc_init(void)
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
	adc_object.adc_periph_1.Instance = ADC1;
	adc_object.adc_periph_1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
	adc_object.adc_periph_1.Init.Resolution = ADC_RESOLUTION_12B;
	adc_object.adc_periph_1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc_object.adc_periph_1.Init.GainCompensation = 0;
	adc_object.adc_periph_1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	adc_object.adc_periph_1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	adc_object.adc_periph_1.Init.LowPowerAutoWait = DISABLE;
	adc_object.adc_periph_1.Init.ContinuousConvMode = DISABLE;
	adc_object.adc_periph_1.Init.NbrOfConversion = 1;
	adc_object.adc_periph_1.Init.DiscontinuousConvMode = DISABLE;
	adc_object.adc_periph_1.Init.DMAContinuousRequests = DISABLE;
	adc_object.adc_periph_1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	adc_object.adc_periph_1.Init.OversamplingMode = DISABLE;

	assert(HAL_ADC_Init(&adc_object.adc_periph_1) == HAL_OK);

	multimode.Mode = ADC_MODE_INDEPENDENT;
	assert(HAL_ADCEx_MultiModeConfigChannel(&adc_object.adc_periph_1, &multimode) == HAL_OK);

//========================================================================
	//IU
	sconfigInjected1.InjectedChannel = ADC_IU_CHANNEL;
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
	assert(HAL_ADCEx_InjectedConfigChannel(&adc_object.adc_periph_1, &sconfigInjected1) == HAL_OK);
//======================================================================
	//IV
	sconfigInjected1.InjectedSamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	sconfigInjected1.InjectedChannel = ADC_IV_CHANNEL;
	sconfigInjected1.InjectedRank = ADC_INJECTED_RANK_2;
	assert(HAL_ADCEx_InjectedConfigChannel(&adc_object.adc_periph_1, &sconfigInjected1) == HAL_OK);

//========================================================================
	//ADC2
	adc_object.adc_periph_2.Instance = ADC2;
	adc_object.adc_periph_2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
	adc_object.adc_periph_2.Init.Resolution = ADC_RESOLUTION_12B;
	adc_object.adc_periph_2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc_object.adc_periph_2.Init.GainCompensation = 0;
	adc_object.adc_periph_2.Init.ScanConvMode = ADC_SCAN_DISABLE;
	adc_object.adc_periph_2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	adc_object.adc_periph_2.Init.LowPowerAutoWait = DISABLE;
	adc_object.adc_periph_2.Init.ContinuousConvMode = DISABLE;
	adc_object.adc_periph_2.Init.NbrOfConversion = 1;
	adc_object.adc_periph_2.Init.DiscontinuousConvMode = DISABLE;
	adc_object.adc_periph_2.Init.DMAContinuousRequests = DISABLE;
	adc_object.adc_periph_2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	adc_object.adc_periph_2.Init.OversamplingMode = DISABLE;
	assert(HAL_ADC_Init(&adc_object.adc_periph_2) == HAL_OK);

//=======================================================================
	//IW
	sconfigInjected2.InjectedChannel = ADC_IW_CHANNEL;
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
	assert(HAL_ADCEx_InjectedConfigChannel(&adc_object.adc_periph_2, &sconfigInjected2) == HAL_OK);

//=======================================================================
	//GPIO IU, IV, IW
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	GPIO_InitStruct.Pin = ADC_IU_GPIO_PIN;
	HAL_GPIO_Init(ADC_IU_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = ADC_IV_GPIO_PIN;
	HAL_GPIO_Init(ADC_IV_GPIO_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = ADC_IW_GPIO_PIN;
	HAL_GPIO_Init(ADC_IW_GPIO_PORT, &GPIO_InitStruct);

	adc_object.adc_currents[ADC_IU_IDX] = (uint16_t*)&(adc_object.adc_periph_1.Instance->JDR1);
	adc_object.adc_currents[ADC_IV_IDX] = (uint16_t*)&(adc_object.adc_periph_1.Instance->JDR2);
	adc_object.adc_currents[ADC_IW_IDX] = (uint16_t*)&(adc_object.adc_periph_2.Instance->JDR1);

	HAL_ADCEx_Calibration_Start(&adc_object.adc_periph_1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&adc_object.adc_periph_2, ADC_SINGLE_ENDED);

    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

	assert(HAL_ADCEx_InjectedStart_IT(&adc_object.adc_periph_1) == HAL_OK && HAL_ADCEx_InjectedStart_IT(&adc_object.adc_periph_2) == HAL_OK);
}

adc_currents_t adc_get_currents(void)
{
	adc_currents_t retVal;
	retVal.curr_a = *(adc_object.adc_currents[ADC_IU_IDX]);
	retVal.curr_b = *(adc_object.adc_currents[ADC_IV_IDX]);
	retVal.curr_c = *adc_object.adc_currents[ADC_IW_IDX];
	return retVal;
}

uint16_t adc_get_signal(adc_signal_t adc_signal)
{
	UNUSED(adc_signal);
	return 0;
}


