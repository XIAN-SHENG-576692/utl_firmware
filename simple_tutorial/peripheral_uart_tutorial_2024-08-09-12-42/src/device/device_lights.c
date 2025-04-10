#include "device_lights.h"

#include <dk_buttons_and_leds.h>

#include <zephyr/logging/log.h>
#define LOG_MODULE_NAME device_lights
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#define RUN_STATUS_LED DK_LED1
#define CON_STATUS_LED DK_LED2

void device_lights_init(void)
{
	int err;

	err = dk_leds_init();
	if (err) {
		LOG_ERR("Cannot init LEDs (err: %d)", err);
	}
}

void device_lights_ble_connection(const bool is_connected)
{
	if(is_connected)
	{
		dk_set_led_on(CON_STATUS_LED);
	}
	else
	{
		dk_set_led_off(CON_STATUS_LED);
	}
}

void device_lights_red_led(const bool value)
{
	dk_set_led(RUN_STATUS_LED, value);
}

void device_lights_error(void)
{
	dk_set_leds_state(DK_ALL_LEDS_MSK, DK_NO_LEDS_MSK);
}