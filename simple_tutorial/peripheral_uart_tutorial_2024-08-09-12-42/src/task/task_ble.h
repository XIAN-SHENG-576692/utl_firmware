#ifndef __TASK_BLE_ON_RECEIVED_H__
#define __TASK_BLE_ON_RECEIVED_H__

#include <stdint.h>
#include <stdbool.h>

void task_ble_on_connection(const bool is_connected);
void task_ble_on_received(const uint8_t *const data, uint16_t len);

#endif