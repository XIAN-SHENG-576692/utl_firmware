#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940_task_adc.h"

int AD5940_TASK_init_impl_zephyr(void);

// ==================================================
// ADC
int AD5940_TASK_ADC_get_access_length_lock(void);
int AD5940_TASK_ADC_release_access_length_lock(void);

int AD5940_TASK_ADC_put_quene(AD5940_TASK_ADC_RESULT adc_result);
int AD5940_TASK_ADC_take_quene(AD5940_TASK_ADC_RESULT *const adc_result);

// ==================================================
// Command
int AD5940_TASK_COMMAND_get_access_measurement_param_lock(void);
int AD5940_TASK_COMMAND_release_access_measurement_param_lock(void);

int AD5940_TASK_COMMAND_wait_measurement(void);
int AD5940_TASK_COMMAND_trigger_measurement(void);

#ifdef __cplusplus
}
#endif
