#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/gatt.h>

int bluetooth_config(void);
void bluetooth_get_bt_gatt_service_static(
    const struct bt_gatt_service_static **services
);

#ifdef __cplusplus
}
#endif
