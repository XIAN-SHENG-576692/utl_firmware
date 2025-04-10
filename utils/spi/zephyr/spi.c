/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "spi.h"

int z_impl_spi_device_init(
	const struct device *const spi
)
{
	int err = 0;
	err = device_is_ready(spi);
	if(!device_is_ready(spi)) {
		printk("SPI master device not ready!\n");
	}
	return !err;
}

int z_impl_spi_cs_init(
	const struct gpio_dt_spec *const cs
)
{
	if(!device_is_ready(cs->port)){
		printk("SPI master chip select device not ready!\n");
	}
    gpio_pin_configure_dt(cs, GPIO_ACTIVE_LOW | GPIO_OUTPUT | GPIO_PULL_UP);
    return 0;
}

int z_impl_spi_read(
    const struct device *const spi, 
    const struct spi_config *const spi_cfg, 
    uint8_t *const rx_buf, 
    const uint32_t buf_len
)
{
	const struct spi_buf rx_spi_buf = {
		.buf = rx_buf,
		.len = buf_len,
	};
	const struct spi_buf_set rx = {
		.buffers = &rx_spi_buf,
		.count = 1
	};

	int error = spi_read(spi, spi_cfg, &rx);
	if(error != 0){
		printk("SPI transceive error: %i\n", error);
		return error;
	}

    return 0;
}

int z_impl_spi_write(
    const struct device *const spi, 
    const struct spi_config *const spi_cfg, 
    uint8_t *const tx_buf, 
    const uint32_t buf_len
)
{
	const struct spi_buf tx_spi_buf = {
		.buf = tx_buf,
		.len = buf_len,
	};
	const struct spi_buf_set tx = {
		.buffers = &tx_spi_buf,
		.count = 1,
	};

	int error = spi_write(spi, spi_cfg, &tx);
	if(error != 0){
		printk("SPI transceive error: %i\n", error);
		return error;
	}

    return 0;
}

int z_impl_spi_transfer(
    const struct device *const spi, 
    const struct spi_config *const spi_cfg, 
    struct k_poll_signal *const spi_done_sig,
    uint8_t *const tx_buf, 
    uint8_t *const rx_buf, 
    const uint32_t buf_len
)
{

	const struct spi_buf tx_spi_buf = {
		.buf = tx_buf,
		.len = buf_len,
	};
	const struct spi_buf_set tx = {
		.buffers = &tx_spi_buf,
		.count = 1,
	};

	struct spi_buf rx_spi_buf = {
		.buf = rx_buf,
		.len = buf_len,
	};
	const struct spi_buf_set rx = {
		.buffers = &rx_spi_buf,
		.count = 1
	};

	// Reset signal
	k_poll_signal_reset(spi_done_sig);
	
	// Start transaction
	int error = spi_transceive_signal(spi, spi_cfg, &tx, &rx, spi_done_sig);
	if(error != 0){
		printk("SPI transceive error: %i\n", error);
		return error;
	}

	// Wait for the done signal to be raised and log the rx buffer
	int spi_signaled, spi_result;
	do{
		k_poll_signal_check(spi_done_sig, &spi_signaled, &spi_result);
	} while(spi_signaled == 0);

	return error;
}

int z_impl_spi_cs_select(
    const struct gpio_dt_spec *const cs, 
    const bool value
)
{
	return gpio_pin_set_dt(cs, !value);
}
