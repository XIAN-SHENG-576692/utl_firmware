#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

int z_impl_spi_device_init(
    const struct device *const spi
);

int z_impl_spi_cs_init(
    const struct gpio_dt_spec *const cs
);

int z_impl_spi_read(
    const struct device *const spi, 
    const struct spi_config *const spi_cfg, 
    uint8_t *const rx_buf, 
    const uint32_t buf_len
);

int z_impl_spi_write(
    const struct device *const spi, 
    const struct spi_config *const spi_cfg, 
    uint8_t *const tx_buf, 
    const uint32_t buf_len
);

int z_impl_spi_transfer(
    const struct device *const spi, 
    const struct spi_config *const spi_cfg, 
    struct k_poll_signal *const spi_done_sig,
    uint8_t *const tx_buf, 
    uint8_t *const rx_buf, 
    const uint32_t buf_len
);

int z_impl_spi_cs_select(
    const struct gpio_dt_spec *const cs, 
    const bool value
);

#ifdef __cplusplus
}
#endif
