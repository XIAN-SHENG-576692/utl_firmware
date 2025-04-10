/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Nordic UART Bridge Service (NUS) sample
 */

#include <zephyr/types.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <soc.h>

#include <stdio.h>

#include <zephyr/logging/log.h>
#define LOG_MODULE_NAME main
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include "device_ble.h"
#include "device_buttons.h"
#include "device_lights.h"
#include "utility_sleep.h"

#define RUN_LED_BLINK_INTERVAL 1000

void error(void)
{
	device_lights_error();

	while (true) {
		/* Spin for ever */
		utility_sleep(1000);
	}
}

int main(void)
{
	int blink_status = 0;
	int err = 0;

	device_lights_init();
	device_buttons_init();

	err = device_ble_init();
	if (err) {
		error();
	}

	for (;;) {
		device_lights_red_led((++blink_status) % 2);
		utility_sleep(RUN_LED_BLINK_INTERVAL);
	}
}