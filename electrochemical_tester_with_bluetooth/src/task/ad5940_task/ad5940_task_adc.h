#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_electrochemical_utils.h"

typedef struct
{
    uint32_t adc_buffer;
    uint16_t adc_buffer_index;
    uint16_t adc_buffer_length;
} AD5940_TASK_ADC_RESULT;

typedef struct
{
    int (*get_access_state_lock)(void);
    int (*release_access_state_lock)(void);

    int (*get_access_length_lock)(void);
    int (*release_access_length_lock)(void);

    int (*put_quene)(AD5940_TASK_ADC_RESULT adc_result);
    int (*take_quene)(AD5940_TASK_ADC_RESULT *const adc_result);

    int (*wait_ad5940_intc_triggered)(void);
} AD5940_TASK_ADC_PORT;

typedef struct
{
    AD5940_TASK_ADC_PORT port;
} AD5940_TASK_ADC_CFG;

AD5940Err AD5940_TASK_ADC_run(AD5940_TASK_ADC_CFG *const cfg);

// ==================================================
// command thread

typedef enum {
    AD5940_TASK_ADC_STATE_UNINITIALIZED,
    AD5940_TASK_ADC_STATE_IDLE,
    AD5940_TASK_ADC_STATE_EXECUTING,
    AD5940_TASK_ADC_STATE_ERROR,
} AD5940_TASK_ADC_STATE;

AD5940_TASK_ADC_STATE AD5940_TASK_ADC_get_state(void);

int AD5940_TASK_ADC_take_result_quene(
    AD5940_TASK_ADC_RESULT *const result
);

#ifdef __cplusplus
}
#endif
