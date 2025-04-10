#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

int Solenoid_Valve_set(const bool value);
int Solenoid_Valve_toggle(void);

#ifdef __cplusplus
}
#endif
