#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940_electrochemical_utils.h"

extern fImpPol_Type test_ad5940_lprtia_result;
extern fImpPol_Type test_ad5940_hsrtia_result;

extern LPDACPara_Type test_ad5940_LPDAC_calibration_result;

extern uint16_t test_ad5940_electrochemical_working_electrode;

/**
 * Refer to pages 50 and 52 of the datasheet.
 * @ref HSTIARTIA_Const
 */
extern uint32_t test_ad5940_HstiaRtiaSel;

#ifdef __cplusplus
}
#endif
