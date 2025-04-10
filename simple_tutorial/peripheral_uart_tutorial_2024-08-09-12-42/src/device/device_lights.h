#ifndef __DEVICE_LIGHTS_H__
#define __DEVICE_LIGHTS_H__

#include <stdbool.h>

void device_lights_init(void);
void device_lights_ble_connection(const bool is_connected);
void device_lights_red_led(const bool value);
void device_lights_error(void);

#endif