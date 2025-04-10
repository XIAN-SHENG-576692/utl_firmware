#include "ad5940.h"
#include "ad5940_port_reset_impl_zephyr.h"

#include <zephyr/drivers/gpio.h>
#include "driver_gpio_impl_zephyr.h"

// static const struct gpio_dt_spec gpio_dt = GPIO_DT_SPEC_GET(
// 	ZEPHYR_USER_PATH, 
// 	ad5941_reset_gpios
// );

int AD5940_Rst_impl_zephyr_init(void)
{
    // int err;

	// err = gpio_pin_configure_dt(&gpio_dt, GPIO_OUTPUT_ACTIVE);
	// if (err < 0) {
	// 	return 0;
	// }

    // return err;

	return 0;
}

void AD5940_RstSet(void)
{
    // gpio_pin_set_dt(&gpio_dt, 1);
	return;
}

void AD5940_RstClr(void)
{
    // gpio_pin_set_dt(&gpio_dt, 0);
	return;
}
