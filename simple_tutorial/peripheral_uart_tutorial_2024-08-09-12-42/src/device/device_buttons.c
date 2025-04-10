#include "device_buttons.h"
#include "task_buttons.h"

#include <dk_buttons_and_leds.h>

#include <zephyr/logging/log.h>
#define LOG_MODULE_NAME device_buttons
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

void device_buttons_init(void)
{
	int err;

	err = dk_buttons_init(task_buttons_on_changed);
	if (err) {
		LOG_ERR("Cannot init buttons (err: %d)", err);
	}
}