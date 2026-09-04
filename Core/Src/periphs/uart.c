#include <uart.h>
#include <string.h>

static uart_t *p_uart = NULL;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == &(p_uart->uart_periph))
	{
		if(Size == p_uart->rx_size)
		{
			mc_callback_execute(&(p_uart->rx_event_callback));
		}
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &(p_uart->uart_periph))
	{
		mc_callback_execute(&(p_uart->tx_event_callback));
	}
}


void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&(p_uart->uart_dma_tx));
}

/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&(p_uart->uart_dma_rx));
}


void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&(p_uart->uart_periph));
}

void uart_init(uart_t *const instance)
{
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();

	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		assert(0);
	}

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

	instance->uart_dma_tx.Instance = DMA1_Channel1;
	instance->uart_dma_tx.Init.Request = DMA_REQUEST_USART2_TX;
	instance->uart_dma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	instance->uart_dma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	instance->uart_dma_tx.Init.MemInc = DMA_MINC_ENABLE;
	instance->uart_dma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	instance->uart_dma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	instance->uart_dma_tx.Init.Mode = DMA_NORMAL;
	instance->uart_dma_tx.Init.Priority = DMA_PRIORITY_LOW;
	if (HAL_DMA_Init(&(instance->uart_dma_tx)) != HAL_OK)
	{
		assert(0);
	}
	__HAL_LINKDMA(&(instance->uart_periph), hdmatx, instance->uart_dma_tx);



	/* USART2_RX Init */
	instance->uart_dma_rx.Instance = DMA1_Channel2;
	instance->uart_dma_rx.Init.Request = DMA_REQUEST_USART2_RX;
	instance->uart_dma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	instance->uart_dma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	instance->uart_dma_rx.Init.MemInc = DMA_MINC_ENABLE;
	instance->uart_dma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	instance->uart_dma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	instance->uart_dma_rx.Init.Mode = DMA_NORMAL;
	instance->uart_dma_rx.Init.Priority = DMA_PRIORITY_LOW;
	if (HAL_DMA_Init(&(instance->uart_dma_rx)) != HAL_OK)
	{
		assert(0);
	}

	__HAL_LINKDMA(&(instance->uart_periph), hdmarx,  instance->uart_dma_rx);


	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	mc_callback_init(&instance->rx_event_callback);
	mc_callback_init(&instance->tx_event_callback);



	p_uart = instance;

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void uart_transmit(uart_t *const instance, const uint8_t *tx_buf, uint16_t tx_buf_size)
{
	assert(tx_buf != NULL);
	assert(tx_buf_size < UART_TX_BUFFER_SIZE);
	memcpy(instance->tx_buffer, tx_buf, tx_buf_size);
	HAL_UART_Transmit_DMA(&(instance->uart_periph), (const uint8_t*)instance->tx_buffer, tx_buf_size);
}

void uart_receive(uart_t *const instance, uint8_t *rx_buf, uint16_t rx_buf_size)
{
	assert(rx_buf != NULL);
	instance->rx_size = rx_buf_size;
	HAL_UARTEx_ReceiveToIdle_DMA(&(instance->uart_periph), rx_buf, rx_buf_size);
}

void uart_register_rx_event_callback(uart_t *const instance, mc_callback_function_t rx_function , mc_callback_param_t param)
{
	mc_callback_register_function(&(instance->rx_event_callback), rx_function, param);
}

void uart_register_tx_event_callback(uart_t *const instance, mc_callback_function_t tx_function, mc_callback_param_t param)
{
	mc_callback_register_function(&(instance->tx_event_callback), tx_function, param);

}


