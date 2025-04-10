#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_electrochemical_utils.h"

typedef struct
{
    uint32_t HstiaRtiaSel;
} TEST_AD5940_ELECTROCHEMICAL_CALIBRATION_PARAMETERS;

AD5940Err test_ad5940_electrochemical_calibration(
    const TEST_AD5940_ELECTROCHEMICAL_CALIBRATION_PARAMETERS *const parameters
);

#ifdef __cplusplus
}
#endif
