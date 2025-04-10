#include "ad5940.h"
#include "ad5940_port_spi_impl_zephyr.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include "driver_gpio_impl_zephyr.h"

#include "spi.h"

static const struct spi_config spi_1_cfg = {
	.operation = 
		SPI_OP_MODE_MASTER
		| SPI_WORD_SET(8)
		| SPI_TRANSFER_MSB,
	.frequency = 6400000,
	.slave = SPI_OP_MODE_MASTER,
};
static struct k_poll_signal spi_1_done_sig = K_POLL_SIGNAL_INITIALIZER(spi_1_done_sig);

static const struct device *const spi_1_device = DEVICE_DT_GET(
	DT_NODELABEL(spi1)
);

static const struct gpio_dt_spec spi_1_cs = GPIO_DT_SPEC_GET(
	DT_PATH(zephyr_user), 
	ad5940_cs_gpios
);

int AD5940_spi_impl_zephyr_init(void)
{
    int err = 0;

	err = z_impl_spi_device_init(spi_1_device);
	if(err)
	{
		return err;
	}
	err = z_impl_spi_cs_init(&spi_1_cs);
	if(err)
	{
		return err;
	}

    return err;
}

void AD5940_CsSet(void)
{
    z_impl_spi_cs_select(&spi_1_cs, true);
	return;
}

void AD5940_CsClr(void)
{
    z_impl_spi_cs_select(&spi_1_cs, false);
	return;
}

void AD5940_ReadWriteNBytes(unsigned char *pSendBuffer, unsigned char *pRecvBuff, unsigned long length)
{
	z_impl_spi_transfer(
		spi_1_device,
		&spi_1_cfg, 
		&spi_1_done_sig, 
		pSendBuffer,
		pRecvBuff,
		length
	);
	return;
}
