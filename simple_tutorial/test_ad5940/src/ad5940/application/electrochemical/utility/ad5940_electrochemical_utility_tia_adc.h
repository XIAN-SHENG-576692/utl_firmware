/**
 * @file    ad5940_electrochemical_utility_tia_adc_loop.h
 * @brief   Configuration functions for TIA and ADC in electrochemical measurements.
 *
 *          This file contains functions for configuring the Low Power TIA (LPTIA) 
 *          and High Speed TIA (HSTIA) measurement loops in conjunction with the ADC.
 *          It provides safeguards and details necessary for proper configuration.
 * 
 * @note    Important: The high-speed DAC (HSDAC) signal chain must NOT be used with the 
 *          low-power TIA (LPTIA). Refer to page 45 of the datasheet for further details.
 *          ```
 *          Note that the high speed DAC signal chain must never be used
 *          in conjunction with the low power TIA. The high speed DAC can
 *          become unstable, leading to incorrect measurements.
 *          ```
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_electrochemical_utility.h"

/**
 * @ref LPTIACfg_Type
 */
typedef struct
{
    uint32_t LpAmpPwrMod;
    #if defined(CHIPSEL_M355)
    uint32_t LpAmpSel;
    #endif
}
AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type;

/**
 * @ref LPTIACfg_Type
 */
typedef struct
{
    uint32_t LpTiaRtia;         /**< Refer to page 37 of the datasheet. 
                                     Ensure the received current does not exceed I_max. 
                                     Use values defined in @ref LPTIARTIA_Const. */
    uint32_t LpTiaRf;
    uint32_t LpTiaRload;        /**< Do not use LPTIARLOAD_SHORT */
}
AD5940_ELECTROCHEMICAL_UTILITY_LPTIACfg_Type;

/**
 * @ref HSTIACfg_Type
 */
typedef struct
{
    uint32_t HstiaRtiaSel;      /**< RTIA selection @ref HSTIARTIA_Const. Refer to pages 50 and 52 of the datasheet for more details. */
    uint32_t ExtRtia;           /**< Value of external RTIA*/
    uint32_t HstiaCtia;         /**< Set internal CTIA value from 1 to 32 pF. Refer to page 52 of the datasheet for more details.*/
    BoolFlag DiodeClose;        /**< Close the switch for internal back to back diode */
    uint32_t HstiaDeRtia;       /**< DE0 node RTIA selection @ref HSTIADERTIA_Const. Refer to Figure 28 (page 50) and Figure 36 (page 76) of the datasheet for more details.*/
    uint32_t HstiaDeRload;      /**< DE0 node Rload selection @ref HSTIADERLOAD_Const. Refer to Figure 28 (page 50) and Figure 36 (page 76) of the datasheet for more details.*/
    #if defined(CHIPSEL_M355)
    uint32_t HstiaDe1Rtia;      /**< (ADuCM355 only, ignored on AD594x)DE1 node RTIA selection @ref HSTIADERTIA_Const */
    uint32_t HstiaDe1Rload;     /**< (ADuCM355 only)DE1 node Rload selection @ref HSTIADERLOAD_Const */
    #endif
}
AD5940_ELECTROCHEMICAL_UTILITY_HSTIACfg_Type;

/**
 * @ref DSPCfg_Type
 */
typedef struct
{
    uint32_t ADCPga;                  /**< ADC PGA settings, select from @ref ADCPGA */
    ADCFilterCfg_Type ADCFilterCfg;   /**< ADC filter configuration include SINC3/SINC2/Notch/Average(for DFT only) */
    ADCDigComp_Type ADCDigCompCfg;    /**< ADC digital comparator */
    DFTCfg_Type DftCfg;               /**< DFT configuration include data source, DFT number and Hanning Window */
    StatCfg_Type StatCfg;             /**< Statistic block */
}
AD5940_ELECTROCHEMICAL_UTILITY_DSPCfg_Type;

/**
 * @brief Configures the Low Power DAC (LPDAC) and Low Power TIA (LPTIA) measurement loop.
 * 
 * @param utility_LPPACfg_Type  Pointer to the utility-specific reference configuration type
 *                              (`AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type`).
 * @param utility_LPTIACfg_Type  Pointer to the utility-specific reference configuration type
 *                              (`AD5940_ELECTROCHEMICAL_UTILITY_LPTIACfg_Type`).
 * @param utility_DSPCfg_Type   Pointer to the utility-specific reference configuration type
 *                              (`AD5940_ELECTROCHEMICAL_UTILITY_DSPCfg_Type`).
 * @return        Returns an `AD5940Err` error code indicating the success or failure of the configuration.
 */
AD5940Err AD5940_ELECTROCHEMICAL_UTILITY_LPDAC_LPTIA_ADC_config(
    const AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type *const utility_LPPACfg_Type,
    const AD5940_ELECTROCHEMICAL_UTILITY_LPTIACfg_Type *const utility_LPTIACfg_Type,
    const AD5940_ELECTROCHEMICAL_UTILITY_DSPCfg_Type *const utility_DSPCfg_Type
);

/**
 * @brief Configures the Low Power DAC (LPDAC) and High Speed TIA (HSTIA) measurement loop.
 * 
 * @param working_electrode     @ref AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE
 * @param utility_LPPACfg_Type  Pointer to the utility-specific reference configuration type
 *                              (`AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type`).
 * @param utility_HSTIACfg_Type Pointer to the utility-specific reference configuration type
 *                              (`AD5940_ELECTROCHEMICAL_UTILITY_HSTIACfg_Type`).
 * @param utility_DSPCfg_Type   Pointer to the utility-specific reference configuration type
 *                              (`AD5940_ELECTROCHEMICAL_UTILITY_DSPCfg_Type`).
 * @return        Returns an `AD5940Err` error code indicating the success or failure of the configuration.
 */
AD5940Err AD5940_ELECTROCHEMICAL_UTILITY_LPDAC_HSTIA_ADC_config(
    const AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE working_electrode,
    const AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type *const utility_LPPACfg_Type,
    const AD5940_ELECTROCHEMICAL_UTILITY_HSTIACfg_Type *const utility_HSTIACfg_Type,
    const AD5940_ELECTROCHEMICAL_UTILITY_DSPCfg_Type *const utility_DSPCfg_Type
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
