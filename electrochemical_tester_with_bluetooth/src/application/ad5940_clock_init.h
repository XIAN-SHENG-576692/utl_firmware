#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_utils.h"

typedef struct
{
    AD5940_ClockConfig clockConfig;
    float lfoscFrequency;
} AD5940_CLOCK_INIT_CTX;

AD5940Err ad5940_clock_init(AD5940_CLOCK_INIT_CTX *ctx);

#ifdef __cplusplus
}
#endif
