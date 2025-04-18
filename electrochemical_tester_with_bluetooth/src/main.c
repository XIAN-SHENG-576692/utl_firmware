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

#include "kernel_sleep.h"
#include "ble_simple_impl_zephyr.h"

#include "ad5940_port_intc0_impl_zephyr.h"
#include "ad5940_port_intc1_impl_zephyr.h"
#include "ad5940_port_reset_impl_zephyr.h"
#include "ad5940_port_spi_impl_zephyr.h"

#include "ad5940_main.h"

#include <zephyr/kernel.h>

#include "test_ad5940_init.h"

#include "test_ad5940_temperature.h"

#include "test_ad5940_electrochemical_calibration.h"

#include "test_ad5940_electrochemical_ca.h"
#include "test_ad5940_electrochemical_cv.h"
#include "test_ad5940_electrochemical_dpv.h"

#define AD5940_CONTROLLER_BUFFER_SIZE 1000
static uint32_t ad5940_controller_buffer[AD5940_CONTROLLER_BUFFER_SIZE];

#define BLE_PACKET_MAX_LENGTH 256
static uint8_t ble_packet_buffer[BLE_PACKET_MAX_LENGTH];
static uint16_t ble_packet_buffer_length;

static TEST_AD5940_ELECTROCHEMICAL_CALIBRATION_PARAMETERS ad5940_electrochemical_calibration_parameters = {
	.HstiaRtiaSel = HSTIARTIA_10K,
};

typedef union {
    ELECTROCHEMICAL_CA ca;
    ELECTROCHEMICAL_CV cv;
    ELECTROCHEMICAL_DPV dpv;
} ELECTROCHEMICAL_PARAMETERS_UNION;

ELECTROCHEMICAL_PARAMETERS_UNION parameters;
AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING electrode_routing;

#include "test_ad5940_buffer_size.h"

#define ADC_BUFFER_SIZE 3000
static uint32_t adc_buffer[ADC_BUFFER_SIZE];
static int16_t adc_buffer_length;

static int32_t temperature;

int main(void)
{
	{
		int err = 0;
	
		err = AD5940_intc0_impl_zephyr_init();
		if (err) return err;
		err = AD5940_intc1_impl_zephyr_init();
		if (err) return err;
		err = AD5940_Rst_impl_zephyr_init();
		if (err) return err;
		err = AD5940_spi_impl_zephyr_init();
		if (err) return err;
	
		KERNEL_SLEEP_ms(1E2);
	
		err = AD5940_MAIN_init(
			ad5940_controller_buffer,
			AD5940_CONTROLLER_BUFFER_SIZE
		);
		if (err) return err;
	
		err = BLE_SIMPLE_IMPL_NRF_init();
		if (err) return err;
		BLE_SIMPLE_IMPL_NRF_wait_inited();
	
		err = test_ad5940_init();
		if (err) return err;
	
		err = test_ad5940_electrochemical_calibration(
			&ad5940_electrochemical_calibration_parameters
		);
		if (err) return err;
	}
	
	while(1) 
	{
		BLE_SIMPLE_wait_new_packet_received();

		BLE_SIMPLE_read_packet(
			BLE_PACKET_MAX_LENGTH,
			ble_packet_buffer,
			&ble_packet_buffer_length
		);
		// Check
		if(ble_packet_buffer[0] != 0x01) 
		{
			continue;
		}

		// Get witch type of electrochemical we want to measure.
		uint8_t electrochemical_type = ble_packet_buffer[1];
		memcpy(
			&parameters.ca,
			(ble_packet_buffer + 2),
			sizeof(ELECTROCHEMICAL_PARAMETERS_UNION)
		);
		memcpy(
			&electrode_routing,
			(ble_packet_buffer + 2 + sizeof(ELECTROCHEMICAL_PARAMETERS_UNION)),
			sizeof(AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING)
		);

		// Measure the temperature then send the result.
		test_ad5940_start_temperature(
			&temperature
		);
		ble_packet_buffer[0] = 0x02;
		ble_packet_buffer[1] = 0x01;
		memcpy((ble_packet_buffer + 2), &temperature, sizeof(temperature));
		BLE_SIMPLE_send_packet(ble_packet_buffer, 6);

		// Measure the electrochemical.
		switch (electrochemical_type)
		{
		case 0x00: 
		{
			test_ad5940_electrochemical_ca_with_LPDAC_HSTIA(
				&parameters.ca,
				&electrode_routing,
				ADC_BUFFER_SIZE,
				adc_buffer,
				&adc_buffer_length
			);
			break;
		}
		case 0x01: 
		{
			test_ad5940_electrochemical_cv_with_LPDAC_HSTIA(
				&parameters.cv,
				&electrode_routing,
				ADC_BUFFER_SIZE,
				adc_buffer,
				&adc_buffer_length
			);
			break;
		}
		case 0x02: 
		{
			test_ad5940_electrochemical_dpv_with_LPDAC_HSTIA(
				&parameters.dpv,
				&electrode_routing,
				ADC_BUFFER_SIZE,
				adc_buffer,
				&adc_buffer_length
			);
			break;
		}
		default:
			break;
		}

		// Send the electrochemical result.
		ble_packet_buffer[0] = 0x02;
		ble_packet_buffer[1] = 0x02;
		for(uint16_t index=0; index<adc_buffer_length; index++)
		{
			memcpy((ble_packet_buffer + 2), &index, sizeof(index));
			int32_t adc_volt = (int32_t) (AD5940_ADCCode2Volt(
				adc_buffer[index] & 0xFFFF, 
				ADCPGA_1, 
				1.82F
			) * 1e9f);
			memcpy((ble_packet_buffer + 2 + sizeof(index)), &adc_volt, sizeof(adc_volt));
			BLE_SIMPLE_send_packet(ble_packet_buffer, (2 + sizeof(index) + sizeof(adc_volt)));
		}
	}

	return 0;
}
