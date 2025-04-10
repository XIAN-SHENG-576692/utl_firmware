#include "task_ble.h"

#include "driver_uart_fifo.h"

void task_ble_on_connection(const bool is_connected)
{
    device_lights_ble_connection(is_connected);
}

void task_ble_on_received(const uint8_t *const data, uint16_t len)
{
    driver_uart_tx_fifo(data, len);
}