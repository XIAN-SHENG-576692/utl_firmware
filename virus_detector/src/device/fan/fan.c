#include "fan.h"

#include "fan_init.h"

#include "driver_gpio_impl_zephyr.h"
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec gpio_dt = GPIO_DT_SPEC_GET(
	ZEPHYR_USER_PATH, 
	fan_gpios
);

int Fan_init(void)
{
    int err;

    // if (!device_is_ready(gpio_dt.port)) {
	// 	return 0;
	// }

	err = gpio_pin_configure_dt(&gpio_dt, GPIO_OUTPUT_INACTIVE);
	if (err < 0) {
		return 0;
	}

    return err;
}

int Fan_set(const bool value)
{
    return gpio_pin_set_dt(&gpio_dt, value);
}

int Fan_toggle(void)
{
    return gpio_pin_toggle_dt(&gpio_dt);
}
