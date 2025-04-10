#ifndef __DRIVER_UART_FIFO_H__
#define __DRIVER_UART_FIFO_H__

#include <stdint.h>
#include <string.h>

#define UART_BUF_SIZE CONFIG_BT_NUS_UART_BUFFER_SIZE

struct uart_data_t {
	void *fifo_reserved;
	uint8_t data[UART_BUF_SIZE];
	uint16_t len;
};

int driver_uart_tx_fifo(const uint8_t* buf, size_t len);

/**
 * @brief Wait indefinitely until data is received.
 */
struct uart_data_t driver_uart_rx_fifo(void);

#endif