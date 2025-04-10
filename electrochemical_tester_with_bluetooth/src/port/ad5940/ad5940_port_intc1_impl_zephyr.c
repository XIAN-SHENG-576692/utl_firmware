#include "ad5940_port_intc1_impl_zephyr.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include <dk_buttons_and_leds.h>
#define AD5940_GPIO7_MASK DK_BTN1_MSK

#include "test_ad5940_lock.h"

// void button_changed(uint32_t button_state, uint32_t has_changed)
// {
// 	uint32_t buttons = button_state & has_changed;

// 	if (buttons & AD5940_GPIO7_MASK) {
// 		test_ad5940_lock_boardcast();
// 	}
// }

int AD5940_intc1_impl_zephyr_init(void)
{
	// int err = 0;

    // err = dk_buttons_init(button_changed);
	// if (err) return err;

    // return err;

    return 0;
}
