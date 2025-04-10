#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"

AD5940Err test_ad5940_start_temperature(
    int32_t *const temperature
);

#ifdef __cplusplus
}
#endif
