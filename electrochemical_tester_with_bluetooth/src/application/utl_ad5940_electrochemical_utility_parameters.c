#include "utl_ad5940_electrochemical_utility_parameters.h"

#include "utl_ad5940_electrochemical_parameters.h"

#include "ad5940_hardware.h"
#include "test_ad5940_parameters.h"

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

/**
 * You need to set the following parameters:
 * - LFOSC_frequency
 */
AD5940_ELECTROCHEMICAL_RUN_CONFIG ad5940_electrochemical_run_config = {
    .agpio_cfg = &AD5940_EXTERNAL_agpio_cfg,
    .clock = &test_ad5940_parameters_clockConfig,
    .DataType = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DataType,
    .FifoSrc = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_FifoSrc,
};

AD5940_ELECTROCHEMICAL_LPDAC_TO_LPTIA_CONFIG ad5940_electrochemical_lpdac_to_lptia_config = {
    .afe_ref_cfg = &utl_ad5940_electrochemical_utility_AFERefCfg_Type,
    .dsp_cfg = &utl_ad5940_electrochemical_utility_DSPCfg_Type,
    .lpdac_cfg = &utl_ad5940_electrochemical_utility_LPPACfg_Type,
    .lptia_cfg = &utl_ad5940_electrochemical_utility_LPTIACfg_Type,
};

/**
 * You need to set the following parameters:
 * - electrode_routing
 */
AD5940_ELECTROCHEMICAL_LPDAC_TO_HSTIA_CONFIG ad5940_electrochemical_lpdac_to_hstia_config = {
    .afe_ref_cfg = &utl_ad5940_electrochemical_utility_AFERefCfg_Type,
    .dsp_cfg = &utl_ad5940_electrochemical_utility_DSPCfg_Type,
    .lpdac_cfg = &utl_ad5940_electrochemical_utility_LPPACfg_Type,
    .hstia_cfg = &utl_ad5940_electrochemical_utility_HSTIACfg_Type,
};

AD5940_ELECTROCHEMICAL_HSDAC_MMR_TO_HSTIA_CONFIG ad5940_electrochemical_hsdac_mmr_to_hstia_config = {0};
