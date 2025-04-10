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
static LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include "ble_simple_impl_zephyr.h"

#include "ad5940_port_intc0_impl_zephyr.h"
#include "ad5940_port_intc1_impl_zephyr.h"
#include "ad5940_port_reset_impl_zephyr.h"
#include "ad5940_port_spi_impl_zephyr.h"

#include "ad5940_init.h"

#include "kernel_sleep.h"

#include "fan.h"
#include "fan_init.h"
#include "pump.h"
#include "pump_init.h"
#include "solenoid_valve.h"
#include "solenoid_valve_init.h"

int main(void)
{
	int err = 0;

	// err = AD5940_intc0_impl_zephyr_init();
	// if (err) return err;
	// err = AD5940_intc1_impl_zephyr_init();
	// if (err) return err;
	// err = AD5940_Rst_impl_zephyr_init();
	// if (err) return err;
	// err = AD5940_spi_impl_zephyr_init();
	// if (err) return err;

	// err = ad5940_init();
	// if(err) return err;

	err = BLE_SIMPLE_IMPL_NRF_init();
	if(err) return err;

	err = Fan_init();
	if(err) return err;

	err = Pump_init();
	if(err) return err;

	err = Solenoid_Valve_init();
	if(err) return err;

	Fan_toggle();
	Pump_toggle();
	Solenoid_Valve_toggle();

	// KERNEL_SLEEP_ms(10000);
	// Fan_toggle();
	// Pump_toggle();
	// Solenoid_Valve_toggle();

	// // Solenoid_Valve_toggle();

	// while (1)
	// {
	// 	// KERNEL_SLEEP_ms(1000);
	// 	Fan_toggle();
	// 	Pump_toggle();
	// 	Solenoid_Valve_toggle();
	// 	KERNEL_SLEEP_ms(500);

	// 	// Fan_set(true);
	// 	// Pump_set(true);
	// 	// Solenoid_Valve_set(true);
	// 	// KERNEL_SLEEP_ms(500);
	// 	// Fan_set(false);
	// 	// Pump_set(false);
	// 	// Solenoid_Valve_set(false);
	// 	// KERNEL_SLEEP_ms(2000);
	// }

	// while (1)
	// {
	// 	Fan_toggle();
	// 	KERNEL_SLEEP_ms(4000);
	// 	Fan_toggle();
	// 	Pump_toggle();
	// 	KERNEL_SLEEP_ms(4000);
	// 	Pump_toggle();
	// 	Solenoid_Valve_toggle();
	// 	KERNEL_SLEEP_ms(4000);
	// 	Solenoid_Valve_toggle();
	// }

	// while (1)
	// {
	// 	Fan_toggle();
	// 	KERNEL_SLEEP_ms(4000);
	// 	Pump_toggle();
	// 	Solenoid_Valve_toggle();
	// }

	// KERNEL_SLEEP_ms(4000);
	// Fan_toggle();
	// Pump_toggle();
	// Solenoid_Valve_toggle();
	// while (1) {}
	
	return 0;
}
