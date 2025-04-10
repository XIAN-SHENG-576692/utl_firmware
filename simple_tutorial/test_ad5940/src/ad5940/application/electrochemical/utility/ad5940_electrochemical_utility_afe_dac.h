#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_electrochemical_utility.h"

/**
 * @ref AFERefCfg_Type
 */
typedef struct
{
}
AD5940_ELECTROCHEMICAL_UTILITY_AFERefCfg_Type;

/**
 * @brief Configures the Low Power DAC (LPDAC) and Low Power TIA (LPTIA) measurement loop.
 * 
 * @param E_start       The starting potential for the measurement (in millivolts).
 * @param E_max         The maximum potential for the measurement (in millivolts).
 * @param utility_type  Pointer to the utility-specific reference configuration type
 *                      (`AD5940_ELECTROCHEMICAL_UTILITY_AFERefCfg_Type`).
 * @return              Returns an `AD5940Err` error code indicating the success or failure of the configuration.
 */
AD5940Err AD5940_ELECTROCHEMICAL_UTILITY_AFE_LPDAC_LPTIA_config(
    const int16_t E_start,
    const int16_t E_max,
    const AD5940_ELECTROCHEMICAL_UTILITY_AFERefCfg_Type *const utility_AFERefCfg_Type
);

/**
 * @brief Configures the Low Power DAC (LPDAC) and High Speed TIA (HSTIA) measurement loop.
 * 
 * @param E_start       The starting potential for the measurement (in millivolts).
 * @param E_max         The maximum potential for the measurement (in millivolts).
 * @param utility_type  Pointer to the utility-specific reference configuration type
 *                      (`AD5940_ELECTROCHEMICAL_UTILITY_AFERefCfg_Type`).
 * @return              Returns an `AD5940Err` error code indicating the success or failure of the configuration.
 */
AD5940Err AD5940_ELECTROCHEMICAL_UTILITY_AFE_LPDAC_HSTIA_config(
    const int16_t E_start, 
    const int16_t E_max,
    const AD5940_ELECTROCHEMICAL_UTILITY_AFERefCfg_Type *const utility_AFERefCfg_Type
);

// /**
//  * @ref AD5940_ELECTROCHEMICAL_STRUCT_get_MMR_HSLoopCfg_Type
//  * @param V_out_peak_to_peak: TDAC output voltage in mV peak to peak. Maximum value is 800mVpp. Peak to peak voltage. (Refer to page 43 and page 103)
// */
// void AD5940_ELECTROCHEMICAL_STRUCT_get_SIN_HSLoopCfg_Type(
//     HSLoopCfg_Type* type, 
//     const uint32_t ExcitBufGain, 
//     const uint32_t HsDacGain, 
//     const uint32_t HsDacUpdateRate, 
//     const uint32_t HstiaRtiaSel,
//     const uint32_t Dswitch,
//     const uint32_t Pswitch,
//     const uint32_t Nswitch,
//     const uint32_t Tswitch,
//     const BoolFlag LPDAC_enable,
//     const float V_out_peak_to_peak,
//     SoftSweepCfg_Type* softSweepCfg_Type,
//     const float sin_frequency,
//     float* sweep_current_frequency,
//     float* sweep_next_frequency,
//     float* frequency_of_latest_data_sampled,
//     const float system_clock_frequency
// );

// type->WgCfg.WgType = WGTYPE_SIN;    // Select from WGTYPE_MMR, WGTYPE_SIN, WGTYPE_TRAPZ. HSDAC is always connected to WG. (Refer to page 99)
// type->WgCfg.GainCalEn = bTRUE;      // Refer to page 100
// type->WgCfg.OffsetCalEn = bTRUE;    // Refer to page 100

#ifdef __cplusplus
}
#endif
