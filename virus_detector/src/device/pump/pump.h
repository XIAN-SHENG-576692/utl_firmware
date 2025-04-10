#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

int Pump_set(const bool value);
int Pump_toggle(void);

#ifdef __cplusplus
}
#endif
