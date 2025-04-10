#include "utl_ad5940_electrochemical_utility_parameters.h"

#include "utl_ad5940_electrochemical_parameters.h"

AD5940_ELECTROCHEMICAL_AFERefCfg_Type utl_ad5940_electrochemical_utility_AFERefCfg_Type = {
};

AD5940_ELECTROCHEMICAL_LPDACfg_Type utl_ad5940_electrochemical_utility_LPPACfg_Type = {
    .LpAmpPwrMod = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpAmpPwrMod,
};

AD5940_ELECTROCHEMICAL_LPTIACfg_Type utl_ad5940_electrochemical_utility_LPTIACfg_Type = {
    .LpTiaRf = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRf,
    .LpTiaRload = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRload,
    .LpTiaRtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRtia,
};

/**
 * You need to set the following parameters:
 * - HstiaRtiaSel
 */
AD5940_ELECTROCHEMICAL_HSTIACfg_Type utl_ad5940_electrochemical_utility_HSTIACfg_Type = {
    .DiodeClose = bFALSE,
    .ExtRtia = bFALSE,
    .HstiaCtia = 31,
    .HstiaDeRload = HSTIADERLOAD_OPEN,
    .HstiaDeRtia = HSTIADERTIA_OPEN,
    // .HstiaRtiaSel = HstiaRtiaSel,
};

/**
 * You need to set the following parameters:
 * - ADCRate
 */
AD5940_ELECTROCHEMICAL_DSPCfg_Type utl_ad5940_electrochemical_utility_DSPCfg_Type = {
    .ADCDigCompCfg = {0},
    .ADCFilterCfg = {
        .ADCAvgNum = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCAvgNum,
        // .ADCRate = ADCRate,
        .ADCSinc2Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc2Osr,
        .ADCSinc3Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc3Osr,
        .BpNotch = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_BpNotch,
        .BpSinc3 = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_BpSinc3,
        // .DFTClkEnable
        // .Sinc2NotchClkEnable,
        .Sinc2NotchEnable = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_Sinc2NotchEnable,
        // .Sinc3ClkEnable
        // .WGClkEnable
    },
    .ADCPga = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCPga,
    .DftCfg = {
        .DftNum = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftNum,
        .DftSrc = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftSrc,
        .HanWinEn = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HanWinEn,
    },
    .StatCfg = {0},
};
