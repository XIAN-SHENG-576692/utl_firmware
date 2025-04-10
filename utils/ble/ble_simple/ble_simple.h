#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

typedef int BLE_SIMPLE_ERROR;

bool BLE_SIMPLE_is_connected(void);

void BLE_SIMPLE_wait_connected(void);
void BLE_SIMPLE_wait_disconnected(void);
void BLE_SIMPLE_wait_connection_state_changed(void);

BLE_SIMPLE_ERROR BLE_SIMPLE_wait_new_packet_received(void);

BLE_SIMPLE_ERROR BLE_SIMPLE_read_packet(
    const uint16_t packet_max_length,
    uint8_t *const packet,
    uint16_t *const packet_length
);

BLE_SIMPLE_ERROR BLE_SIMPLE_send_packet(
    const uint8_t *const packet, 
    const uint16_t packet_length
);

uint16_t BLE_SIMPLE_get_packet_max_length(void);

#ifdef __cplusplus
}
#endif
