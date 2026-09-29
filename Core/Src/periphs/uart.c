#include <uart.h>
#include <string.h>

static uart_t *p_uart = NULL;
static DMA_HandleTypeDef dma_usart_tx;
static DMA_HandleTypeDef dma_usart_rx;

void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(p_uart->uart_dma_tx);
}

void DMA1_Channel2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(p_uart->uart_dma_tx);
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&p_uart->uart_periph);
}

void uart_init(uart_t *const instance)
{
	__HAL_RCC_DMAMUX1_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

	instance->uart_periph.Instance = USART2;
	instance->uart_periph.Init.BaudRate = 921600;
	instance->uart_periph.Init.WordLength = UART_WORDLENGTH_8B;
	instance->uart_periph.Init.StopBits = UART_STOPBITS_1;
	instance->uart_periph.Init.Parity = UART_PARITY_NONE;
	instance->uart_periph.Init.Mode = UART_MODE_TX_RX;
	instance->uart_periph.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	instance->uart_periph.Init.OverSampling = UART_OVERSAMPLING_16;
	instance->uart_periph.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	instance->uart_periph.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	instance->uart_periph.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&instance->uart_periph) != HAL_OK)
	{
		assert(0);
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&instance->uart_periph, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		assert(0);
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&instance->uart_periph, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		assert(0);
	}
	if (HAL_UARTEx_DisableFifoMode(&instance->uart_periph) != HAL_OK)
	{
		assert(0);
	}

	instance->uart_dma_tx = &dma_usart_tx;
	instance->uart_dma_rx = &dma_usart_rx;

	p_uart = instance;

	HAL_UARTEx_ReceiveToIdle_DMA(&(instance->uart_periph), instance->rx_buf, UART_RX_BUFFER_SIZE);

	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void uart_transmit(uart_t *const instance, const uint8_t *tx_data, uint16_t tx_data_size)
{
	assert(tx_data != NULL);
	assert(tx_data_size < UART_TX_BUFFER_SIZE);
	HAL_UART_Transmit_DMA(&instance->uart_periph, (const uint8_t*)tx_data, tx_data_size);
}

const uint8_t* uart_get_rx_data(uart_t *const instance)
{
	uint8_t *ret_val = instance->rx_buf;
	return ret_val;
}

uint16_t uart_get_rx_data_size(uart_t *const instance)
{
	uint16_t ret_val = instance->rx_size;
	return ret_val;
}

void uart_register_rx_event_callback(uart_t *const instance, mc_callback_function_t rx_function , mc_callback_param_t param)
{
	mc_callback_register_function(&(instance->rx_event_callback), rx_function, param);
}

void uart_register_tx_event_callback(uart_t *const instance, mc_callback_function_t tx_function, mc_callback_param_t param)
{
	mc_callback_register_function(&(instance->tx_event_callback), tx_function, param);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	if (huart->Instance == USART2)
	{
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
		PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			assert(0);
		}

		__HAL_RCC_USART2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		dma_usart_tx.Instance = DMA1_Channel1;
		dma_usart_tx.Init.Request = DMA_REQUEST_USART2_TX;
		dma_usart_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		dma_usart_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		dma_usart_tx.Init.MemInc = DMA_MINC_ENABLE;
		dma_usart_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		dma_usart_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		dma_usart_tx.Init.Mode = DMA_NORMAL;
		dma_usart_tx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&dma_usart_tx) != HAL_OK)
		{
			assert(0);
		}

		__HAL_LINKDMA(huart, hdmatx, dma_usart_tx);

		dma_usart_rx.Instance = DMA1_Channel2;
		dma_usart_rx.Init.Request = DMA_REQUEST_USART2_RX;
		dma_usart_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		dma_usart_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		dma_usart_rx.Init.MemInc = DMA_MINC_ENABLE;
		dma_usart_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		dma_usart_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		dma_usart_rx.Init.Mode = DMA_NORMAL;
		dma_usart_rx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&dma_usart_rx) != HAL_OK)
		{
			assert(0);
		}
		__HAL_LINKDMA(huart, hdmarx, dma_usart_rx);

	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	UNUSED(huart);
	p_uart->rx_size = Size;
    mc_callback_execute(&(p_uart->rx_event_callback));
    HAL_UARTEx_ReceiveToIdle_DMA(&(p_uart->uart_periph), p_uart->rx_buf, UART_RX_BUFFER_SIZE);
}


