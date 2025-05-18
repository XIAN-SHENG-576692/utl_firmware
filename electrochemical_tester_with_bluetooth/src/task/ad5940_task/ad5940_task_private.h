#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940_task_adc.h"
#include "ad5940_task_command.h"

int AD5940_TASK_ADC_reset(
    uint8_t flag,
    uint16_t length
);

#ifdef __cplusplus
}
#endif
