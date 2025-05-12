/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Nordic UART Bridge Service (NUS) sample
 */

#include <stdio.h>

#include <soc.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>

#include <zephyr/logging/log.h>
#define LOG_MODULE_NAME main
static LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include "ble_simple_impl_zephyr.h"

#include "ad5940_port_intc0_impl_zephyr.h"
#include "ad5940_port_intc1_impl_zephyr.h"
#include "ad5940_port_reset_impl_zephyr.h"
#include "ad5940_port_spi_impl_zephyr.h"

#include "ad5940_intc0_lock_impl_zephyr.h"
#include "ad5940_intc0_lock.h"

#include "ad5940_main.h"

#define AD5940_CONTROLLER_BUFFER_SIZE 1000
static uint32_t ad5940_controller_buffer[AD5940_CONTROLLER_BUFFER_SIZE];

#define BLE_PACKET_MAX_LENGTH 256
static uint8_t ble_packet_buffer[BLE_PACKET_MAX_LENGTH];
static uint16_t ble_packet_buffer_length;

// ==================================================
// Watch Dog
#include "watchdog0_wdt.h"

// ==================================================
// AD5940 initialize parameters
#include "ad5940_clock_init.h"
static AD5940_CLOCK_INIT_CTX ad5940_clock_init_ctx = {
};

#include "ad5940_electrochemical_calibration.h"

#include "utl_ad5940_electrochemical_parameters.h"

// Our circuit use it.
#define MAIN_AD5940_HSTIARTIA HSTIARTIA_10K

static AD5940_ELECTROCHEMICAL_CALIBRATION_PARAMETERS ad5940_electrochemical_calibration_parameters = {
	.HstiaRtiaSel = MAIN_AD5940_HSTIARTIA,

	// UTL parameters
	.ADCRefVolt = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCRefVolt,
	.SettleTime10us = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_SettleTime10us,
	.TimeOut10us = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_TimeOut10us,
	.ADCPga = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCPga,
	.PGACalType = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_PGACalType,
	.VRef1p11 = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_VRef1p11,
	.VRef1p82 = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_VRef1p82,
	.ADCSinc2Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc2Osr,
	.ADCSinc3Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc3Osr,
	.DftNum = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftNum,
	.DftSrc = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftSrc,
	.HanWinEn = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HanWinEn,
	.DiodeClose = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DiodeClose,
	.HstiaCtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HstiaCtia,
	.HstiaDeRload = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HstiaDeRload,
	.HstiaDeRtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HstiaDeRtia,
	.LpAmpPwrMod = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpAmpPwrMod,
	.LpAmpSel = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpAmpSel,
	.LpTiaRtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRtia,
	.bWithCtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_bWithCtia,
};

static AD5940_ELECTROCHEMICAL_CALIBRATION_RESULTS ad5940_electrochemical_calibration_results = {
};

// ==================================================
// AD5940 Application

#include "ad5940_hardware.h"
#include "ad5940_application_impl_zephyr.h"

// ADC
#include "ad5940_application_adc.h"

static k_tid_t ad5940_application_adc_tid;
static struct k_thread ad5940_application_adc_thread;
K_THREAD_STACK_DEFINE(ad5940_application_adc_stack, 4096);

AD5940_APPLICATION_ADC_CFG ad5940_application_adc_cfg = {
	.port = {
		.wait_ad5940_intc_triggered = ad5940_intc0_lock_wait,
		.get_access_state_lock = ad5940_application_adc_get_access_state_lock,
		.release_access_state_lock = ad5940_application_adc_release_access_state_lock,
		.get_access_length_lock = ad5940_application_adc_get_access_length_lock,
		.release_access_length_lock = ad5940_application_adc_release_access_length_lock,
		.put_quene = ad5940_application_adc_put_quene,
		.take_quene = ad5940_application_adc_take_quene,
	},
};

// Command
#include "ad5940_application_command.h"

static k_tid_t ad5940_application_command_tid;
static struct k_thread ad5940_application_command_thread;
K_THREAD_STACK_DEFINE(ad5940_application_command_stack, 16384);

AD5940_APPLICATION_COMMAND_CFG ad5940_application_command_cfg = {
	.port = {
		.get_access_state_lock = ad5940_application_command_get_access_state_lock,
		.release_access_state_lock = ad5940_application_command_release_access_state_lock,
		.get_access_measurement_param_lock = ad5940_application_command_get_access_measurement_param_lock,
		.release_access_measurement_param_lock = ad5940_application_command_release_access_measurement_param_lock,
		.trigger_measurement = ad5940_application_command_trigger_measurement,
		.wait_measurement = ad5940_application_command_wait_measurement,
	},
	.param = {
		.HstiaRtiaSel = MAIN_AD5940_HSTIARTIA,
		
		// UTL parameters
		// You need to set the following parameters:
		// - ADCRate
		.ADCRefVolt = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCRefVolt,
		.ADCPga = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCPga,
		.ADCSinc2Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc2Osr,
		.ADCSinc3Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc3Osr,
		.DftNum = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftNum,
		.DftSrc = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftSrc,
		.HanWinEn = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HanWinEn,
		.LpAmpPwrMod = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpAmpPwrMod,
		.LpTiaRtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRtia,

		.ADCAvgNum = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCAvgNum,

		.BpNotch = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_BpNotch,
		.BpSinc3 = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_BpSinc3,
		.Sinc2NotchEnable = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_Sinc2NotchEnable,

		.LpTiaRf = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRf,
		.LpTiaRload = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRload,

		.DataType = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DataType,
		.FifoSrc = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_FifoSrc,
	},
};

// ==================================================
// Command Receiver
#include "command_receiver.h"
#include "command_receiver_impl_zephyr.h"

static k_tid_t command_receiver_tid;
static struct k_thread command_receiver_thread;
K_THREAD_STACK_DEFINE(command_receiver_stack, 4096);

int wait_command_received(
	uint8_t **const command,
	uint16_t *const command_length
) 
{
	BLE_SIMPLE_ERROR err = 0;

	err = BLE_SIMPLE_wait_new_packet_received();
	if(err) return err;

	err = BLE_SIMPLE_read_packet(
		BLE_PACKET_MAX_LENGTH,
		ble_packet_buffer,
		&ble_packet_buffer_length
	);
	if(err) return err;

	*command = ble_packet_buffer;
	*command_length = ble_packet_buffer_length;

	return err;
}

COMMAND_RECEIVER_CFG command_receiver_cfg = {
	.port = {
		.get_access_state_lock = command_receiver_get_access_state_lock,
		.release_access_state_lock = command_receiver_release_access_state_lock,
		.send_response = BLE_SIMPLE_send_packet,
		.wait_command_received = wait_command_received,
	},
	.param = {
		.ad5940_application_command_param = &ad5940_application_command_cfg.param,
	},
};

// ==================================================
// AD5940 ADC Sender

static k_tid_t ad5940_adc_sender_tid;
static struct k_thread ad5940_adc_sender_thread;
K_THREAD_STACK_DEFINE(ad5940_adc_sender_stack, 4096);

int ad5940_adc_sender(void)
{
	int err;
	AD5940_APPLICATION_ADC_RESULT result;
	#define AD5940_ADC_SENDER_LENGTH (1 + 1 + sizeof(AD5940_APPLICATION_ADC_RESULT))
	uint8_t ble_packet[AD5940_ADC_SENDER_LENGTH];
	ble_packet[0] = 0x02;
	ble_packet[1] = 0x01;
	for(;;)
	{
		err = ad5940_application_adc_take_result_quene(
			&result
		);
		memcpy(
			ble_packet + 2,
			&result,
			sizeof(result)
		);
		err = BLE_SIMPLE_send_packet(
			ble_packet,
			AD5940_ADC_SENDER_LENGTH
		);
		if(err) return err;
	}
}

int main(void)
{
	{
		int err = 0;

		err = ad5940_intc0_lock_init_impl_zephyr();
		if (err) return err;

		err = AD5940_intc0_impl_zephyr_init(ad5940_intc0_lock_boardcast_impl_zephyr);
		if (err) return err;
		err = AD5940_intc1_impl_zephyr_init();
		if (err) return err;
		err = AD5940_Rst_impl_zephyr_init();
		if (err) return err;
		err = AD5940_spi_impl_zephyr_init();
		if (err) return err;
	
    	k_sleep(K_MSEC(1e2));
	
		err = AD5940_MAIN_init(
			ad5940_controller_buffer,
			AD5940_CONTROLLER_BUFFER_SIZE
		);
		if (err) return err;
	
		err = BLE_SIMPLE_IMPL_NRF_init();
		if (err) return err;
		BLE_SIMPLE_IMPL_NRF_wait_inited();
		
		err = watchdog0_init();
		if (err) return err;
	
		// ==================================================
		// AD5940 initialize parameters
		err = ad5940_clock_init(
			&ad5940_clock_init_ctx
		);
		if (err) return err;
		ad5940_electrochemical_calibration_parameters.clockConfig = ad5940_clock_init_ctx.clockConfig;
		ad5940_electrochemical_calibration_parameters.lfoscFrequency = ad5940_clock_init_ctx.lfoscFrequency;

		err = ad5940_electrochemical_calibration(
			&ad5940_electrochemical_calibration_parameters,
			&ad5940_electrochemical_calibration_results
		);
		if (err) return err;

		// ==================================================
		// AD5940 application

		ad5940_application_init_impl_zephyr();

		// ADC
		ad5940_application_adc_tid = k_thread_create(
			&ad5940_application_adc_thread,
			ad5940_application_adc_stack,
			K_THREAD_STACK_SIZEOF(ad5940_application_adc_stack),
			ad5940_application_adc_run,
			&ad5940_application_adc_cfg, NULL, NULL,
			5, 0,
			K_NO_WAIT
		);

		// Command
		ad5940_application_command_cfg.param.agpio_cfg = AD5940_EXTERNAL_agpio_cfg;

		ad5940_application_command_cfg.param.lprtia_calibration_result = ad5940_electrochemical_calibration_results.lprtia_calibration_result;
		ad5940_application_command_cfg.param.hsrtia_calibration_result = ad5940_electrochemical_calibration_results.hsrtia_calibration_result;

		ad5940_application_command_cfg.param.clockConfig = ad5940_clock_init_ctx.clockConfig;
		ad5940_application_command_cfg.param.lfoscFrequency = ad5940_clock_init_ctx.lfoscFrequency;

		ad5940_application_command_cfg.param.ADCRate = ad5940_clock_init_ctx.clockConfig.ADCRate;

		ad5940_application_command_tid = k_thread_create(
			&ad5940_application_command_thread,
			ad5940_application_command_stack,
			K_THREAD_STACK_SIZEOF(ad5940_application_command_stack),
			ad5940_application_command_run,
			&ad5940_application_command_cfg, NULL, NULL,
			5, 0,
			K_NO_WAIT
		);

		// ==================================================
		// Command Receiver

		command_receiver_init_impl_zephyr();

		command_receiver_tid = k_thread_create(
			&command_receiver_thread,
			command_receiver_stack,
			K_THREAD_STACK_SIZEOF(command_receiver_stack),
			command_receiver_run,
			&command_receiver_cfg, NULL, NULL,
			5, 0,
			K_NO_WAIT
		);

		// ==================================================
		// AD5940 ADC Sender

		ad5940_adc_sender_tid = k_thread_create(
			&ad5940_adc_sender_thread,
			ad5940_adc_sender_stack,
			K_THREAD_STACK_SIZEOF(ad5940_adc_sender_stack),
			ad5940_adc_sender,
			NULL, NULL, NULL,
			5, 0,
			K_NO_WAIT
		);
	}

	watchdog0_feed();
	for(;;)
	{
    	k_sleep(K_MSEC(4.5e3));
		watchdog0_feed();
	}

	return 0;
}
