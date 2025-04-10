#include "task_thread.h"

#include <zephyr/kernel.h>

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/logging/log.h>
#define LOG_MODULE_NAME task_thread
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include "device_ble.h"
#include "driver_uart_fifo.h"
#include "device_ble_waiting_inited.h"

#define STACKSIZE CONFIG_BT_NUS_THREAD_STACK_SIZE
#define PRIORITY 7

void task_thread_init(void)
{
    return;
}

void ble_write_from_uart_thread(void)
{
	device_ble_waiting_inited();

	for (;;) 
    {
		/* Wait indefinitely for data to be sent over bluetooth */
		struct uart_data_t buf = driver_uart_rx_fifo();

        if (device_ble_send(buf.data, buf.len)) {
            LOG_WRN("Failed to send data over BLE connection");
        }
	}
}

K_THREAD_DEFINE(
    ble_write_thread_id, 
    STACKSIZE, 
    ble_write_from_uart_thread, 
    NULL, 
    NULL,
    NULL, 
    PRIORITY, 
    0, 
    0
);