#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"

#include "ad5940_electrochemical_cv_struct.h"

typedef struct 
{
    AD5940_ELECTROCHEMICAL_CV_PARAMETERS ad5940_parameters;
    uint8_t number_of_scans;
} 
ELECTROCHEMICAL_CV;

AD5940Err test_ad5940_electrochemical_cv_with_LPDAC_LPTIA(
    const ELECTROCHEMICAL_CV *const parameters,
    const int16_t adc_buffer_max_length,
    uint32_t *const adc_buffer,
    int16_t *const adc_buffer_length
);

AD5940Err test_ad5940_electrochemical_cv_with_LPDAC_HSTIA(
    const ELECTROCHEMICAL_CV *const parameters,
    const AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING *const electrode_routing,
    const int16_t adc_buffer_max_length,
    uint32_t *const adc_buffer,
    int16_t *const adc_buffer_length
);

#ifdef __cplusplus
}
#endif
