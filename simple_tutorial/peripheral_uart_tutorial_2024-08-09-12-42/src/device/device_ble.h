#ifndef __DEVICE_BLE_H__
#define __DEVICE_BLE_H__

#include <stdint.h>
#include <stdbool.h>

int device_ble_init(void);

int device_ble_send(const uint8_t* data, uint32_t length);

#ifdef CONFIG_BT_NUS_SECURITY_ENABLED
void device_ble_auth_conn(const bool accept);
#endif

#endif