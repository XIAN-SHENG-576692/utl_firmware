#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_electrochemical_utils.h"

typedef struct
{
    uint16_t adc_buffer_index;
    uint16_t adc_buffer_length;
    uint32_t adc_buffer;
} AD5940_TASK_ADC_RESULT;

// ==================================================
// PORT
int AD5940_TASK_ADC_get_access_length_lock(void);
int AD5940_TASK_ADC_release_access_length_lock(void);

int AD5940_TASK_ADC_put_quene(const AD5940_TASK_ADC_RESULT *const adc_result);
int AD5940_TASK_ADC_take_quene(AD5940_TASK_ADC_RESULT *const adc_result);

int AD5940_TASK_ADC_wait_ad5940_intc_triggered(void);
// ==================================================

typedef struct
{
    void (*start)(void);
    void (*end)(void);
} AD5940_TASK_ADC_CALLBACK;

typedef struct
{
    AD5940_TASK_ADC_CALLBACK callback;
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
