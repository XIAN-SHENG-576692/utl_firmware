/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Nordic UART Bridge Service (NUS) sample
 */

#include <stdio.h>
#include <stdatomic.h>
#include <soc.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>

#include <zephyr/logging/log.h>
#define LOG_MODULE_NAME main
static LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include "ble_simple_impl_zephyr.h"
#include "kernel_sleep.h"

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
#include "watchdog0.h"

// ==================================================
// AD5940 initialize parameters
#include "ad5940_clock_init.h"
static AD5940_CLOCK_INIT_CTX ad5940_clock_init_ctx = {
};

#include "ad5940_electrochemical_calibration.h"

#include "utl_ad5940_electrochemical_parameters.h"
#include "utl_ad5940_temperature_parameters.h"

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
// AD5940 TASK

#include "ad5940_hardware.h"
#include "ad5940_task_impl_zephyr.h"

// ADC
#include "ad5940_task_adc.h"

static k_tid_t ad5940_task_adc_tid;
static struct k_thread ad5940_task_adc_thread;
K_THREAD_STACK_DEFINE(ad5940_task_adc_stack, 4096);

volatile static atomic_uint_fast8_t AD5940_TASK_ADC_heartbeat_count = 0;
void AD5940_TASK_ADC_add_heartbeat(void)
{
	atomic_fetch_add(&AD5940_TASK_ADC_heartbeat_count, 1);
	return;
}
uint8_t AD5940_TASK_ADC_read_heartbeat(void)
{
    return atomic_load(&AD5940_TASK_ADC_heartbeat_count);
}

int AD5940_TASK_ADC_wait_ad5940_intc_triggered(void)
{
	return ad5940_intc0_lock_wait();
}

AD5940_TASK_ADC_CFG ad5940_task_adc_cfg = {
	.callback = {
		.end = AD5940_TASK_ADC_add_heartbeat,
		.start = AD5940_TASK_ADC_add_heartbeat,
	},
};

// Command
#include "ad5940_task_command.h"

static k_tid_t ad5940_task_command_tid;
static struct k_thread ad5940_task_command_thread;
K_THREAD_STACK_DEFINE(ad5940_task_command_stack, 16384);

volatile static atomic_uint_fast8_t AD5940_TASK_COMMAND_heartbeat_count = 0;
void AD5940_TASK_COMMAND_add_heartbeat(void)
{
	atomic_fetch_add(&AD5940_TASK_COMMAND_heartbeat_count, 1);
	return;
}
uint8_t AD5940_TASK_COMMAND_read_heartbeat(void)
{
    return atomic_load(&AD5940_TASK_COMMAND_heartbeat_count);
}

AD5940_TASK_COMMAND_CFG ad5940_task_command_cfg = {
	.callback = {
		.end = AD5940_TASK_COMMAND_add_heartbeat,
		.start = AD5940_TASK_COMMAND_add_heartbeat,
	},
	.param = {
		.common = {
		},
		// UTL parameters
		// You need to set the following parameters:
		// - ADCRate
		.electrochemical = {
			.HstiaRtiaSel = MAIN_AD5940_HSTIARTIA,
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
		// UTL parameters
		// You need to set the following parameters:
		// - clockConfig
		// - lfoscFrequency
		.temperature = {
			.ADCAvgNum = UTL_AD5940_TEMPERATURE_PARAMETERS_ADCAvgNum,
			.ADCPga = UTL_AD5940_TEMPERATURE_PARAMETERS_ADCPga,
			.ADCSinc2Osr = UTL_AD5940_TEMPERATURE_PARAMETERS_ADCSinc2Osr,
			.ADCSinc3Osr = UTL_AD5940_TEMPERATURE_PARAMETERS_ADCSinc3Osr,
			.BpNotch = UTL_AD5940_TEMPERATURE_PARAMETERS_BpNotch,
			.BpSinc3 = UTL_AD5940_TEMPERATURE_PARAMETERS_BpSinc3,
			.DataType = UTL_AD5940_TEMPERATURE_PARAMETERS_DataType,
			.FifoSrc = UTL_AD5940_TEMPERATURE_PARAMETERS_FifoSrc,
			.Sinc2NotchEnable = UTL_AD5940_TEMPERATURE_PARAMETERS_Sinc2NotchEnable,
		},
	},
};

// ==================================================
// Command Receiver
#include "command_receiver.h"
#include "command_receiver_impl_zephyr.h"

static k_tid_t command_receiver_tid;
static struct k_thread command_receiver_thread;
K_THREAD_STACK_DEFINE(command_receiver_stack, 4096);

volatile static atomic_uint_fast8_t COMMAND_RECEIVER_heartbeat_count = 0;
void COMMAND_RECEIVER_add_heartbeat(void)
{
	atomic_fetch_add(&COMMAND_RECEIVER_heartbeat_count, 1);
	return;
}
uint8_t COMMAND_RECEIVER_read_heartbeat(void)
{
    return atomic_load(&COMMAND_RECEIVER_heartbeat_count);
}

static atomic_uint_fast32_t entity_id = 0;
int COMMAND_RECEIVER_wait_command_received(
    COMMAND_RECEIVER_START *const start
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

	if(ble_packet_buffer[0] != 0x01) return 0;

	start->type = (COMMAND_RECEIVER_START_TYPE) ble_packet_buffer[1];

	switch (start->type)
	{
	case COMMAND_RECEIVER_START_TYPE_STOP:
		break;
	case COMMAND_RECEIVER_START_TYPE_ELECTROCHEMICAL_CA:
	case COMMAND_RECEIVER_START_TYPE_ELECTROCHEMICAL_CV:
	case COMMAND_RECEIVER_START_TYPE_ELECTROCHEMICAL_DPV:
		uint8_t *p0 = ble_packet_buffer + 2;
		uint8_t *p1 = p0;
		atomic_store(&entity_id, *((uint32_t *) p1));
		p1 += sizeof(entity_id);
		memcpy(&start->param.electrochemical.parameters, p1, sizeof(start->param.electrochemical.parameters));
		p1 += sizeof(start->param.electrochemical.parameters);
		memcpy(&start->param.electrochemical.routing, p1, sizeof(start->param.electrochemical.routing));
		p1 += sizeof(start->param.electrochemical.routing);
		break;
	default:
		break;
	}
	return err;
}

int COMMAND_RECEIVER_send_response(
    uint8_t *const command,
    const uint16_t command_length
)
{
	return BLE_SIMPLE_send_packet(command, command_length);
}

COMMAND_RECEIVER_CFG command_receiver_cfg = {
	.callback = {
		.end = COMMAND_RECEIVER_add_heartbeat,
		.start = COMMAND_RECEIVER_add_heartbeat,
	},
	.param = {
		.ad5940_task_command_param = &ad5940_task_command_cfg.param,
	},
};

// ==================================================
// AD5940 ADC Sender

static k_tid_t ad5940_adc_sender_tid;
static struct k_thread ad5940_adc_sender_thread;
K_THREAD_STACK_DEFINE(ad5940_adc_sender_stack, 4096);

volatile static atomic_uint_fast8_t AD5940_ADC_SENDER_heartbeat_count = 0;
void AD5940_ADC_SENDER_add_heartbeat(void)
{
	atomic_fetch_add(&AD5940_ADC_SENDER_heartbeat_count, 1);
	return;
}
uint8_t AD5940_ADC_SENDER_read_heartbeat(void)
{
    return atomic_load(&AD5940_ADC_SENDER_heartbeat_count);
}

int AD5940_ADC_SENDER_run(void)
{
	int err;
	AD5940_TASK_ADC_RESULT result;
	#define AD5940_ADC_SENDER_LENGTH (1 + sizeof(entity_id) + 1 + sizeof(result.adc_data_index) + sizeof(result.fifo_buffer[0]))
	uint8_t ble_packet[AD5940_ADC_SENDER_LENGTH];
	ble_packet[0] = 0x02;
	for(;;)
	{
		err = AD5940_TASK_ADC_take_result_quene(
			&result
		);
		if(!BLE_SIMPLE_is_connected()) continue;

		uint8_t *p = ble_packet + 1;

		uint_fast32_t id = atomic_load(&entity_id);
		memcpy(p, &id, sizeof(id));
		p += sizeof(id);

		uint8_t flag = (uint8_t) result.flag;
		memcpy(p, &flag, sizeof(flag));
		p += sizeof(flag);

		memcpy(p, &result.adc_data_index, sizeof(result.adc_data_index));
		p += sizeof(result.adc_data_index);

		switch (result.flag)
		{
		case AD5940_TASK_ADC_RESULT_FLAG_TEMPERATURE:
		{
			AD5940_convert_adc_to_temperature(
				result.fifo_buffer[0],
				UTL_AD5940_TEMPERATURE_PARAMETERS_ADCPga,
				(int32_t *) p
			);
			p += sizeof(int32_t);
			break;
		}
		case AD5940_TASK_ADC_RESULT_FLAG_HSTIA_CURRENT:
		{
			AD5940_convert_adc_to_current(
				result.fifo_buffer[0],
				&ad5940_task_command_cfg.param.electrochemical.hsrtia_calibration_result,
				UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCPga,
				UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCRefVolt,
				(int32_t *) p
			);
			p += sizeof(int32_t);
			break;
		}
		default:
			break;
		}

		err = BLE_SIMPLE_send_packet(
			ble_packet,
			AD5940_ADC_SENDER_LENGTH
		);
		// if(err) return err;
	}
}

// ==================================================
// Watch dog timer
typedef struct
{
    uint8_t last_heartbeat;
    uint8_t error_count;
    const uint8_t error_count_allow_max;
} WDT_ERROR_CHECKER;

int WDT_update_checker(
	WDT_ERROR_CHECKER *const checker,
	const uint8_t new_heartbeat
)
{
	if(checker->last_heartbeat == new_heartbeat)
	{
		if(checker->error_count < checker->error_count_allow_max)
		{
			checker->error_count++;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		checker->error_count = 0;
		checker->last_heartbeat = new_heartbeat;
	}
	return 0;
}

int main(void)
{
	int err = 0;
	{
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
	
		err = AD5940_MAIN_init(
			ad5940_controller_buffer,
			AD5940_CONTROLLER_BUFFER_SIZE,
			2
		);
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
		// AD5940 task

		AD5940_TASK_init_impl_zephyr();

		// ADC
		ad5940_task_adc_tid = k_thread_create(
			&ad5940_task_adc_thread,
			ad5940_task_adc_stack,
			K_THREAD_STACK_SIZEOF(ad5940_task_adc_stack),
			AD5940_TASK_ADC_run,
			&ad5940_task_adc_cfg, NULL, NULL,
			5, 0,
			K_NO_WAIT
		);

		// Command

		// common
		ad5940_task_command_cfg.param.common.agpio_cfg = AD5940_EXTERNAL_agpio_cfg;

		// electrochemical
		ad5940_task_command_cfg.param.electrochemical.lprtia_calibration_result = ad5940_electrochemical_calibration_results.lprtia_calibration_result;
		ad5940_task_command_cfg.param.electrochemical.hsrtia_calibration_result = ad5940_electrochemical_calibration_results.hsrtia_calibration_result;

		ad5940_task_command_cfg.param.electrochemical.clockConfig = ad5940_clock_init_ctx.clockConfig;
		ad5940_task_command_cfg.param.electrochemical.lfoscFrequency = ad5940_clock_init_ctx.lfoscFrequency;

		ad5940_task_command_cfg.param.electrochemical.ADCRate = ad5940_clock_init_ctx.clockConfig.ADCRate;

		// temperature
		ad5940_task_command_cfg.param.temperature.clockConfig = ad5940_clock_init_ctx.clockConfig;
		ad5940_task_command_cfg.param.temperature.lfoscFrequency = ad5940_clock_init_ctx.lfoscFrequency;

		ad5940_task_command_tid = k_thread_create(
			&ad5940_task_command_thread,
			ad5940_task_command_stack,
			K_THREAD_STACK_SIZEOF(ad5940_task_command_stack),
			AD5940_TASK_COMMAND_run,
			&ad5940_task_command_cfg, NULL, NULL,
			5, 0,
			K_NO_WAIT
		);

		// ==================================================
		// Command Receiver

		COMMAND_RECEIVER_init_impl_zephyr();

		command_receiver_tid = k_thread_create(
			&command_receiver_thread,
			command_receiver_stack,
			K_THREAD_STACK_SIZEOF(command_receiver_stack),
			COMMAND_RECEIVER_run,
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
			AD5940_ADC_SENDER_run,
			NULL, NULL, NULL,
			5, 0,
			K_NO_WAIT
		);
	}
	
	err = watchdog0_init();
	if (err) return err;

	{
		// Since some AD5940 FIFOs may be blocked upon first use after power-on, 
		// this operation is necessary to ensure proper functionality.
		KERNEL_SLEEP_ms(100);
		err = watchdog0_feed();
		if (err) return err;
		AD5940_TASK_MEASUREMENT_PARAM param = {
			.type = AD5940_TASK_TYPE_TEMPERATURE,
			.param.temperature = {
				.sampling_interval = 0.01,
				.sampling_time = 0.01,
				.TEMPSENS = 0,
			},
		};
		for(size_t i=0; i<4; i++)
		{
			AD5940_TASK_COMMAND_measure(&param);
			KERNEL_SLEEP_ms(100);
			err = watchdog0_feed();
			if (err) return err;
		}
	}
	
	// BLE
	err = BLE_SIMPLE_IMPL_NRF_init();
	if (err) return err;
	BLE_SIMPLE_IMPL_NRF_wait_inited();

	// ==================================================
	// Watch dog timer
	
	err = watchdog0_feed();
	if (err) return err;
	
	WDT_ERROR_CHECKER wdt_ad5940_task_adc_checker = {
		.last_heartbeat = 0,
		.error_count = 0,
		.error_count_allow_max = 1,
	};
	WDT_ERROR_CHECKER wdt_ad5940_task_command_checker = {
		.last_heartbeat = 0,
		.error_count = 0,
		.error_count_allow_max = 1,
	};
	WDT_ERROR_CHECKER command_receiver_checker = {
		.error_count = 0,
		.last_heartbeat = 0,
		.error_count_allow_max = 3,
	};
	for(;;)
	{
		// ==================================================
		// AD5940 task
		// ADC
		if(AD5940_TASK_ADC_get_state() == AD5940_TASK_ADC_STATE_IDLE)
		{
			wdt_ad5940_task_adc_checker.error_count = 0;
			wdt_ad5940_task_adc_checker.last_heartbeat = AD5940_TASK_ADC_read_heartbeat();
		}
		else
		{
			err = WDT_update_checker(
				&wdt_ad5940_task_adc_checker,
				AD5940_TASK_ADC_read_heartbeat()
			);
			if(err) return err;
		}
		// Command
		if(AD5940_TASK_COMMAND_get_state() == AD5940_TASK_COMMAND_STATE_IDLE)
		{
			wdt_ad5940_task_command_checker.error_count = 0;
			wdt_ad5940_task_command_checker.last_heartbeat = AD5940_TASK_COMMAND_read_heartbeat();
		}
		else
		{
			err = WDT_update_checker(
				&wdt_ad5940_task_command_checker,
				AD5940_TASK_COMMAND_read_heartbeat()
			);
			if(err) return err;
		}
		// ==================================================
		// Command Receiver
		if(COMMAND_RECEIVER_get_state() == COMMAND_RECEIVER_STATE_IDLE)
		{
			command_receiver_checker.error_count = 0;
			command_receiver_checker.last_heartbeat = COMMAND_RECEIVER_read_heartbeat();
		}
		else
		{
			err = WDT_update_checker(
				&command_receiver_checker,
				COMMAND_RECEIVER_read_heartbeat()
			);
			if(err) return err;
		}
    	k_sleep(K_MSEC(4.5e2));
		err = watchdog0_feed();
		if(err) return err;
	}

	return 0;
}
