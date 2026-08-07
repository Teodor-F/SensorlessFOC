/*---------------- Includes --------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <hall.h>

/*---------------- Private defines -------------------------------------------*/
#define NO_OF_EXAMINED_ITEMS 	2u
#define INIT_VALUE           	0xFFFFFFFFU
#define INVALID_POSITION 		6U

#define MAX_HALL_OBJ			1u
#define HALL_PERIPH_0 			0u

#define HALL_1_SHIFT_VAL           0u
#define HALL_2_SHIFT_VAL_REGULAR   1u
#define HALL_3_SHIFT_VAL_REGULAR   2u
#define HALL_2_SHIFT_VAL_SWAPPED   2u
#define HALL_3_SHIFT_VAL_SWAPPED   1u

#define HALL_SECTORS 6u

#define D_MC_HALL_1_GPIO_PORT 		GPIOA
#define D_MC_HALL_1_PIN				GPIO_PIN_15
#define D_MC_HALL_1_PIN_POS			15u
#define D_MC_HALL_2_GPIO_PORT		GPIOB
#define D_MC_HALL_2_PIN				GPIO_PIN_3
#define D_MC_HALL_2_PIN_POS			3u
#define D_MC_HALL_3_GPIO_PORT		GPIOB
#define D_MC_HALL_3_PIN				GPIO_PIN_10
#define D_MC_HALL_3_PIN_POS			10u

/*---------------- Private typedefs ------------------------------------------*/
typedef enum hall_signal_polarity hall_signal_polarity_t;
typedef enum hall_sensor_swap hall_sensor_swap_t;
typedef enum hall_index hall_index_t;
typedef struct hall_gpio hall_gpio_t;
typedef struct hall hall_t;
typedef struct hall_periph hall_periph_t;

/*---------------- Private enums ---------------------------------------------*/

enum hall_index
{
    HALL_1 = 0u,
    HALL_2,
    HALL_3,
    MAX_HALL
};


enum hall_signal_polarity
{
	NONINVERTED,
	INVERTED
};

enum hall_sensor_swap{
	HALL_2_3_REGULAR,
	HALL_2_3_SWAPPED
};

/*---------------- Private macros --------------------------------------------*/

/*---------------- Private structs -------------------------------------------*/

struct hall_gpio
{
	GPIO_TypeDef* gpioPort;
	uint16_t gpioPin;
	uint16_t pinPos;
};

struct hall_periph
{
	TIM_HandleTypeDef tim1;
	hall_gpio_t hall1;
	hall_gpio_t hall2;
	hall_gpio_t hall3;
};

struct hall
{
	hall_periph_t periph;
	uint8_t defaultNumberOfMotorPoles;
	uint8_t numberOfMotorPoles;

	hall_signal_polarity_t hallSignalPolarityConfiguration;
	hall_signal_polarity_t hallSignalPolarityActual;
	uint32_t errorCnt;
	uint32_t positionCounter;
	uint32_t electricalPosition;
	uint32_t lastRawHallPos;
	uint32_t validationCnt;
	uint32_t lastValidPos;
	uint32_t lastInvalidPos;
	bool activateErrorBehavior;
	uint32_t capturedPosition;
	hall_sensor_swap_t swapState;
	uint32_t hall2ShiftValue;
	uint32_t hall3ShiftValue;
	uint8_t moduleIsEnabled;
};

/*---------------- Private variables & constants -----------------------------*/

static hall_t self = {0};

static const uint32_t actHallTable[8U] =
{
	// Hall-sector idx <--> Hall-sector signals values (HS3, HS2, HS1)
    INVALID_POSITION, 		// 000
    2U,               		// 001
    4U,               		// 010
    3U,               		// 011
    0U,               		// 100
    1U,               		// 101
    5U,               		// 110
    INVALID_POSITION  		// 111
};

static const uint32_t previousHallTableCW[7U] =
{
	// Hall-sector order <--> 5 → 4 → 3 → 2 → 1 → 0
    5U, // 0
    0U, // 1
    1U, // 2
    2U, // 3
    3U, // 4
    4U, // 5
    INVALID_POSITION
};

static const uint32_t previousHallTableCCW[7U] =
{
	// Hall-sector order <--> 1 → 2 → 3 → 4 → 5 → 0
    1U, // 0
    2U, // 1
    3U, // 2
    4U, // 3
    5U, // 4
    0U, // 5
    INVALID_POSITION
};

/*---------------- Public module variable & constants definitions ------------*/

/*---------------- Private function declarations -----------------------------*/
static uint32_t checkHallSignals(hall_t *const instance);
static void resetValues(hall_t* const this);
/*---------------- Private function definitions ------------------------------*/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(self.periph.tim1.Instance == htim->Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		uint32_t valCount = checkHallSignals(&self);
		if (valCount == NO_OF_EXAMINED_ITEMS)
		{
			__NOP();
			// execute position change callback
		}
	}
}

static uint32_t checkHallSignals(hall_t *const this)
{
	uint32_t actualPos = 0u;
	uint32_t tmpRawVal = 0u;
	uint32_t rawHallPos = 0U;
	const uint32_t shiftValue[MAX_HALL] = { 0u, this->hall2ShiftValue, this->hall3ShiftValue };

	uint32_t hallGpioPort = this->periph.hall1.gpioPort->IDR;
	uint16_t hallGpioPin = this->periph.hall1.gpioPin;
	uint16_t hallGpioPos = this->periph.hall1.pinPos;
	tmpRawVal = READ_BIT(hallGpioPort, hallGpioPin);
	tmpRawVal = (tmpRawVal >> hallGpioPos);
	tmpRawVal = (tmpRawVal << shiftValue[0]);
	rawHallPos |= tmpRawVal;

	hallGpioPort = this->periph.hall2.gpioPort->IDR;
	hallGpioPin = this->periph.hall2.gpioPin;
	hallGpioPos = this->periph.hall2.pinPos;
	tmpRawVal = READ_BIT(hallGpioPort, hallGpioPin);
	tmpRawVal = (tmpRawVal >> hallGpioPos);
	tmpRawVal = (tmpRawVal << shiftValue[1]);
	rawHallPos |= tmpRawVal;

	hallGpioPort = this->periph.hall3.gpioPort->IDR;
	hallGpioPin = this->periph.hall3.gpioPin;
	hallGpioPos = this->periph.hall3.pinPos;
	tmpRawVal = READ_BIT(hallGpioPort, hallGpioPin);
	tmpRawVal = (tmpRawVal >> hallGpioPos);
	tmpRawVal = (tmpRawVal << shiftValue[2]);
	rawHallPos |= tmpRawVal;

	if (this->hallSignalPolarityActual == INVERTED)
	{
		rawHallPos = (~rawHallPos) & 0x07U;
	}

	// Based on the HS3, HS2, HS1 bits select the sector idx
	actualPos = actHallTable[rawHallPos];

	if (this->lastRawHallPos != rawHallPos)
	{
		// if in the last steps error has not occurs, check and set the position
		if (this->validationCnt == NO_OF_EXAMINED_ITEMS)
		{
			// CW
			if ((this->electricalPosition) == previousHallTableCW[actualPos])
			{
				// add one to the global position counter
				this->positionCounter++;
			}
			// CCW
			else if ((this->electricalPosition) == previousHallTableCCW[actualPos])
			{
				// subtract one from the global position counter
				this->positionCounter--;
			}
			// error is happened after valid position
			else
			{
				// check that it is in initial status and has valid hall position or not
				if ((this->lastRawHallPos != INIT_VALUE) || (actualPos == INVALID_POSITION))
				{
					// reset validation counter
					this->validationCnt = 0U;
				}
			}
		}
		// error handling after invalid position
		else
		{
			// check the actual position's validity
			if ((actualPos != INVALID_POSITION)
					&& ((this->lastInvalidPos == previousHallTableCW[actualPos])
							|| (this->lastValidPos == previousHallTableCW[actualPos])
							|| (this->lastInvalidPos == previousHallTableCCW[actualPos])
							|| (this->lastValidPos == previousHallTableCCW[actualPos])))
			{
				// increase validation counter
				this->validationCnt++;
			}
			// check if the new value also fault
			else
			{
				// reset validation counter
				this->validationCnt = 0U;
			}
		}
		// valid position change is happened
		if (this->validationCnt == NO_OF_EXAMINED_ITEMS)
		{
			// save the actual Hall position
			this->electricalPosition = actualPos;
			// save the actual position than last valid position
			this->lastValidPos = actualPos;
		}
		// error happened
		else if (this->validationCnt == 0U)
		{
			// save the actual position as last invalid position
			this->lastInvalidPos = actualPos;
			// increase the error counter
			this->errorCnt++;
		}
		// revalidation case
		else
		{
			// save last valid position
			this->lastValidPos = actualPos;
		}
		// assign the actual raw hall positions to last raw hall position
		this->lastRawHallPos = rawHallPos;
	}
	return this->validationCnt;
}

static void resetValues(hall_t* const this)
{
    // assign INIT_VALUE to electrical position
   this->electricalPosition = INIT_VALUE;
    // assign 0 to position counter
   this->positionCounter = 0U;
    // assign 0 to error counter
   this->errorCnt = 0U;
    // assign default value to error happened counter register
   this->validationCnt = NO_OF_EXAMINED_ITEMS;
    // assign INVALID_POSITION to last invalid position
   this->lastInvalidPos = INVALID_POSITION;
    // assign INVALID_POSITION to last valid position
   this->lastValidPos = INVALID_POSITION;
    // assign INIT_VALUE to last position - important for the initial signal verification
   this->lastRawHallPos = INIT_VALUE;
   this->swapState = HALL_2_3_REGULAR;
   this->hall2ShiftValue = HALL_2_SHIFT_VAL_REGULAR;
   this->hall3ShiftValue = HALL_3_SHIFT_VAL_REGULAR;
    // assign 0 to the captured position
   this->capturedPosition = 0u;
    // reset the actual "this" pointer's values
   this->periph.hall1.gpioPort = 0U;
   this->periph.hall2.gpioPort = 0U;
   this->periph.hall3.gpioPort = 0U;
}


void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&self.periph.tim1);
}

/*---------------- Public module function definitions ------------------------*/

void hall_init()
{
	resetValues(&self);
//========================================================================
	//TIM2 Hall-sensor mode
	__HAL_RCC_TIM2_CLK_ENABLE();

	TIM_HandleTypeDef *htim1 = &self.periph.tim1;
	memset(htim1, 0, sizeof(TIM_HandleTypeDef));

	TIM_ClockConfigTypeDef sClockSourceConfig =	{0};
	TIM_HallSensor_InitTypeDef sConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim1->Instance = TIM2;
	htim1->Init.Prescaler = 0;
	htim1->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1->Init.Period = (UINT32_MAX - 1);
	htim1->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(htim1) != HAL_OK)
	{
		assert(0);
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(htim1, &sClockSourceConfig) != HAL_OK)
	{
		assert(0);
	}
	sConfig.IC1Polarity = TIM_ICPOLARITY_BOTHEDGE;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.Commutation_Delay = 0;
	if (HAL_TIMEx_HallSensor_Init(htim1, &sConfig) != HAL_OK)
	{
		assert(0);
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC2REF;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(htim1, &sMasterConfig) != HAL_OK)
	{
		assert(0);
	}

//========================================================================
	//TIM2 Hall-sensor pins
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

	//Configure GPIO pin : D_MC_HALL_1_PIN
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = D_MC_HALL_1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(D_MC_HALL_1_GPIO_PORT, &GPIO_InitStruct);

	//Configure GPIO pin : D_MC_HALL_2_PIN
    GPIO_InitStruct.Pin = D_MC_HALL_2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(D_MC_HALL_2_GPIO_PORT, &GPIO_InitStruct);

	//Configure GPIO pin : D_MC_HALL_3_PIN
	GPIO_InitStruct.Pin = D_MC_HALL_3_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(D_MC_HALL_3_GPIO_PORT, &GPIO_InitStruct);

	self.periph.hall1.gpioPort = D_MC_HALL_1_GPIO_PORT;
	self.periph.hall1.gpioPin = D_MC_HALL_1_PIN;
	self.periph.hall1.pinPos = D_MC_HALL_1_PIN_POS;

	self.periph.hall2.gpioPort = D_MC_HALL_2_GPIO_PORT;
	self.periph.hall2.gpioPin = D_MC_HALL_2_PIN;
	self.periph.hall2.pinPos = D_MC_HALL_2_PIN_POS;

	self.periph.hall3.gpioPort = D_MC_HALL_3_GPIO_PORT;
	self.periph.hall3.gpioPin = D_MC_HALL_3_PIN;
	self.periph.hall3.pinPos = D_MC_HALL_3_PIN_POS;

	self.swapState = HALL_2_3_SWAPPED;
	self.hall2ShiftValue = HALL_2_SHIFT_VAL_SWAPPED;
	self.hall3ShiftValue = HALL_3_SHIFT_VAL_SWAPPED;
	self.hallSignalPolarityConfiguration = INVERTED;
	self.hallSignalPolarityActual = INVERTED;

	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	if (HAL_TIMEx_HallSensor_Start_IT(&self.periph.tim1) != HAL_OK)
	{
	    assert(0);
	}

	UNUSED(checkHallSignals(&self));
}

uint32_t hall_getPosition(void)
{
   uint32_t retVal = 0u;
   retVal = self.positionCounter;
   return retVal;
}

uint32_t hall_getActElectricalPosition(void)
{
   uint32_t retVal = 0u;

   if(self.electricalPosition != INIT_VALUE)
   {
	   retVal = self.electricalPosition;
   }
   return retVal;
}

float hall_getElectricalAngle(void)
{
	float angle = 0.0f;
	angle = ((((float)self.electricalPosition) + 0.5f) * (CONSTANT_TWO_PI / (float)HALL_SECTORS));
	if(angle >= CONSTANT_TWO_PI)
	{
		angle -= CONSTANT_TWO_PI;
	}

	angle = CONSTANT_TWO_PI - angle;
	if(angle >= CONSTANT_TWO_PI)
	{
		angle -= CONSTANT_TWO_PI;
	}

	return angle;
}
