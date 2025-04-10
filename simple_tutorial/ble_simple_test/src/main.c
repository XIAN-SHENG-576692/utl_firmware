#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <soc.h>

#include <bluetooth/services/nus.h>

#include <dk_buttons_and_leds.h>

#include <zephyr/settings/settings.h>

#include <stdio.h>

#include <zephyr/logging/log.h>

#include "ble_simple_impl_nrf.h"
#include "kernel_sleep.h"

#define LOG_MODULE_NAME peripheral_uart
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#define RUN_STATUS_LED DK_LED1
#define RUN_LED_BLINK_INTERVAL 1000

#define BLE_TEST_THREAD_STACK_SIZE CONFIG_BT_NUS_THREAD_STACK_SIZE
#define BLE_TEST_THREAD_PRIORITY 7

#define BLE_PACKET_MAX_LENGTH 300
uint8_t ble_packet[BLE_PACKET_MAX_LENGTH]; 
uint16_t ble_packet_length; 

static void error(void)
{
	dk_set_leds_state(DK_ALL_LEDS_MSK, DK_NO_LEDS_MSK);

	while (true) {
		KERNEL_SLEEP_forever();
	}
}

static void configure_gpio(void)
{
	int err;
	err = dk_leds_init();
	if (err) {
		LOG_ERR("Cannot init LEDs (err: %d)", err);
	}
}

int main(void)
{
	int blink_status = 0;
	int err = 0;

	configure_gpio();

	err = BLE_SIMPLE_IMPL_NRF_init();
	if (err) {
		error();
	}

	LOG_INF("Bluetooth initialized");

	for (;;) {
		dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);
		k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));
	}
}

static void ble_test_thread(void)
{
	/* Don't go any further until BLE is initialized */
	BLE_SIMPLE_IMPL_NRF_wait_inited();

	for (;;) {
		int error = 0;

		error = BLE_SIMPLE_wait_new_packet_received();
		if(error) continue;

		error = BLE_SIMPLE_read_packet(
			ble_packet, 
			BLE_PACKET_MAX_LENGTH,
			&ble_packet_length
		);
		if(error) continue;

		error = BLE_SIMPLE_send_packet(
			ble_packet,
			ble_packet_length
		);
		if(error) continue;
	}
}

K_THREAD_DEFINE(
	ble_test_thread_id, 
	BLE_TEST_THREAD_STACK_SIZE, 
	ble_test_thread, 
	NULL, 
	NULL,
	NULL, 
	BLE_TEST_THREAD_PRIORITY, 
	0, 
	0
);
