#ifndef CONFIG_HARDWARE_H_
#define CONFIG_HARDWARE_H_

#include "stm32g4xx_hal.h"

#define SYS_CLK_HZ					170000000U


//========================================================================
	// PWM
#define PWM_TIM						TIM1
#define PWM_TIM_CLK_HZ				SYS_CLK_HZ
#define PWM_FREQ_HZ         		16000u
#define PWM_UH_GPIO_PORT    		GPIOA
#define PWM_UH_GPIO_PIN				GPIO_PIN_8   /* TIM1_CH1  */
#define PWM_VH_GPIO_PORT			GPIOA
#define PWM_VH_GPIO_PIN     		GPIO_PIN_9   /* TIM1_CH2  */
#define PWM_WH_GPIO_PORT    		GPIOA
#define PWM_WH_GPIO_PIN     		GPIO_PIN_10  /* TIM1_CH3  */
#define PWM_UL_GPIO_PORT   			GPIOA
#define PWM_UL_GPIO_PIN     		GPIO_PIN_7   /* TIM1_CH1N */
#define PWM_VL_GPIO_PORT    		GPIOB
#define PWM_VL_GPIO_PIN     		GPIO_PIN_0   /* TIM1_CH2N */
#define PWM_WL_GPIO_PORT    		GPIOB
#define PWM_WL_GPIO_PIN     		GPIO_PIN_1   /* TIM1_CH3N */
#define PWM_ADC_TRIG_DBG_GPIO_PIN	GPIO_PIN_3
#define	PWM_ADC_TRIG_DBG_GPIO_PORT	GPIOC
#define PWM_ADC_TRIG_DBG_AF			GPIO_AF2_TIM1
#define PWM_GPIO_PIN_AF				GPIO_AF6_TIM1

#define PWM_TIM_CLK_ENABLE() 			\
	do									\
	{									\
		__HAL_RCC_TIM1_CLK_ENABLE();	\
	}									\
	while(0);

#define PWM_GPIO_PIN_CLK_ENABLE() 		\
	do									\
	{									\
		__HAL_RCC_GPIOA_CLK_ENABLE();	\
		__HAL_RCC_GPIOB_CLK_ENABLE();	\
		__HAL_RCC_GPIOC_CLK_ENABLE();	\
	}									\
	while(0);

//========================================================================
	// ADC

#define ADC_IU_GPIO_PIN		GPIO_PIN_0
#define ADC_IU_GPIO_PORT    GPIOA
#define ADC_IU_CHANNEL      ADC_CHANNEL_1

#define ADC_IV_GPIO_PIN		GPIO_PIN_1
#define ADC_IV_GPIO_PORT    GPIOC
#define ADC_IV_CHANNEL      ADC_CHANNEL_7

#define ADC_IW_GPIO_PIN		GPIO_PIN_0
#define ADC_IW_GPIO_PORT    GPIOC
#define ADC_IW_CHANNEL      ADC_CHANNEL_6

#define ADC_CLK_ENABLE() 				\
	do									\
	{									\
		__HAL_RCC_ADC12_CLK_ENABLE();	\
	}									\
	while(0)

#define ADC_GPIO_PIN_CLK_ENABLE() 		\
	do									\
	{									\
		__HAL_RCC_GPIOA_CLK_ENABLE();	\
		__HAL_RCC_GPIOC_CLK_ENABLE();	\
	}									\
	while(0)

//========================================================================
	// MCTIMER
#define MC_TIM					TIM3
#define MC_TIM_CLK_ENABLE() 			\
	do									\
	{									\
		__HAL_RCC_TIM3_CLK_ENABLE();	\
	}									\
	while(0)

#define MC_TIM_EVENT1_DBG_PIN	GPIO_PIN_8
#define MC_TIM_EVENT1_DBG_PORT	GPIOC

#define MC_TIM_EVENT2_DBG_PIN	GPIO_PIN_9
#define MC_TIM_EVENT2_DBG_PORT	GPIOC

//========================================================================
	// UART
#define UART_COMM_REFRESH_RATE_HZ	25u
#define UART_COMM_TRIG_CNT_VALUE	PWM_FREQ_HZ / UART_COMM_REFRESH_RATE_HZ

//========================================================================
	// OTHER SYSTEM CONSTANTS

#define TS ((float)(1.0f/PWM_FREQ_HZ))

#endif /* CONFIG_HARDWARE_H_ */
