#ifndef UART_H_
#define UART_H_

#define UART_RX_BUFFER_SIZE 128u
#define UART_TX_BUFFER_SIZE	128u

#include <mc_callback.h>

typedef struct uart uart_t;

struct uart {
	UART_HandleTypeDef uart_periph;
	DMA_HandleTypeDef *uart_dma_tx;
	DMA_HandleTypeDef *uart_dma_rx;
	mc_callback_t tx_event_callback;
	mc_callback_t rx_event_callback;
	uint16_t rx_size;
	bool_t tx_pending;
	uint8_t rx_buf[UART_RX_BUFFER_SIZE];
};


void uart_init(uart_t *const instance);

void uart_transmit(uart_t *const instance, const uint8_t *tx_data, uint16_t tx_data_size);

const uint8_t* uart_get_rx_data(uart_t *const instance);

uint16_t uart_get_rx_data_size(uart_t *const instance);

void uart_register_rx_event_callback(uart_t *const instance, mc_callback_function_t rx_function , mc_callback_param_t param);

void uart_register_tx_event_callback(uart_t *const instance, mc_callback_function_t rx_callback, mc_callback_param_t param);

#endif /* UART_H_ */
