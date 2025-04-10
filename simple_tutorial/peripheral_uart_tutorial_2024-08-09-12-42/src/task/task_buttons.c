#include "task_buttons.h"

#include <dk_buttons_and_leds.h>

#ifdef CONFIG_BT_NUS_SECURITY_ENABLED
#include "device_ble.h"
#define KEY_PASSKEY_ACCEPT DK_BTN1_MSK
#define KEY_PASSKEY_REJECT DK_BTN2_MSK
#endif

void task_buttons_on_changed(uint32_t button_state, uint32_t has_changed)
{
	uint32_t buttons = button_state & has_changed;

	#ifdef CONFIG_BT_NUS_SECURITY_ENABLED
	if (buttons & KEY_PASSKEY_ACCEPT) {
		device_ble_auth_conn(true);
	}
	if (buttons & KEY_PASSKEY_REJECT) {
		device_ble_auth_conn(false);
	}
	#endif
}