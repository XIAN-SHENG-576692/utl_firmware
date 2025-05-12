#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940_application_adc.h"

int ad5940_application_init_impl_zephyr(void);

// ==================================================
// ADC
int ad5940_application_adc_get_access_state_lock(void);
int ad5940_application_adc_release_access_state_lock(void);

int ad5940_application_adc_get_access_length_lock(void);
int ad5940_application_adc_release_access_length_lock(void);

int ad5940_application_adc_put_quene(AD5940_APPLICATION_ADC_RESULT adc_result);
int ad5940_application_adc_take_quene(AD5940_APPLICATION_ADC_RESULT *const adc_result);

// ==================================================
// Command
int ad5940_application_command_get_access_state_lock(void);
int ad5940_application_command_release_access_state_lock(void);

int ad5940_application_command_get_access_measurement_param_lock(void);
int ad5940_application_command_release_access_measurement_param_lock(void);

int ad5940_application_command_wait_measurement(void);
int ad5940_application_command_trigger_measurement(void);

#ifdef __cplusplus
}
#endif
