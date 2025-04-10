#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ble_simple.h"

BLE_SIMPLE_ERROR BLE_SIMPLE_IMPL_NRF_init(void);
bool BLE_SIMPLE_IMPL_NRF_is_inited(void);
void BLE_SIMPLE_IMPL_NRF_wait_inited(void);

#ifdef __cplusplus
}
#endif
