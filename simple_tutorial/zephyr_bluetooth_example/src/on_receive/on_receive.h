#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <zephyr/bluetooth/gatt.h>

ssize_t on_receive(
    struct bt_conn *conn, 
    const struct bt_gatt_attr *attr,
    const void *buf, 
    uint16_t len, 
    uint16_t offset, 
    uint8_t flags
);

#ifdef __cplusplus
}
#endif
