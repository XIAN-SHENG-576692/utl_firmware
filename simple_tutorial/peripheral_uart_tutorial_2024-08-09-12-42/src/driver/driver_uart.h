#ifndef __DRIVER_UART_H__
#define __DRIVER_UART_H__

#include <stdint.h>
#include <string.h>

int driver_uart_init(void);

int driver_uart_tx(const uint8_t* buf, size_t len);
int driver_uart_rx(uint8_t* buf, size_t len);

#endif