#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

int Fan_set(const bool value);
int Fan_toggle(void);

#ifdef __cplusplus
}
#endif
