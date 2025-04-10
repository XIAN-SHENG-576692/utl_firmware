#include <zephyr/bluetooth/gatt.h>

#define BLUETOOTH_SEND_RANDOM_BYTES_THREAD_STACK_SIZE CONFIG_BT_NUS_THREAD_STACK_SIZE
#define BLUETOOTH_SEND_RANDOM_BYTES_THREAD_PRIORITY 7

/* GATT service - data handler */
#define MAX_DATA_LEN 20  // Define maximum packet length
static uint8_t rx_buffer[MAX_DATA_LEN];

#include "bluetooth_config.h"
#include "bluetooth_config_lock.h"

#include <stdlib.h>

static void bluetooth_send_random_bytes_thread(void)
{
	/* Don't go any further until BLE is initialized */
	bluetooth_config_lock_wait_ready();

    const struct bt_gatt_service_static *services;
	const struct bt_gatt_attr *attr; 
    bluetooth_get_bt_gatt_service_static(
        &services
    );
	attr = &services->attrs[7];

	for (;;) {
        /* Send random bytes */
		for(int i = 0; i < MAX_DATA_LEN; i++) {
			rx_buffer[i] = rand() % 256;
		}

        int err = bt_gatt_notify(NULL, attr, rx_buffer, MAX_DATA_LEN);
        if (err) {
            printk("Failed to send notification (err %d)\n", err);
        } else {
            printk("Processed data sent\n");
        }
		k_sleep(K_MSEC(1000));
	}
}

K_THREAD_DEFINE(
	bluetooth_send_random_bytes_thread_id, 
	BLUETOOTH_SEND_RANDOM_BYTES_THREAD_STACK_SIZE, 
	bluetooth_send_random_bytes_thread, 
	NULL, 
	NULL,
	NULL, 
	BLUETOOTH_SEND_RANDOM_BYTES_THREAD_PRIORITY, 
	0, 
	0
);
