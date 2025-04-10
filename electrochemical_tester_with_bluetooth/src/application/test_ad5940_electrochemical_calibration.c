#include "test_ad5940_electrochemical_calibration.h"

#include "test_ad5940_electrochemical_calibration_result.h"

#include "test_ad5940_parameters.h"
#include "test_ad5940_electrochemical_parameters.h"

#include "utl_ad5940_electrochemical_parameters.h"

#include "ad5940_hardware.h"

static ADCPGACal_Type ADCPGACal;
static LPTIAOffsetCal_Type LPTIAOffsetCal;
static LPRTIACal_Type LPRTIACal;
static HSRTIACal_Type HSRTIACal;
static LPDACCal_Type LPDACCal;

AD5940Err test_ad5940_electrochemical_calibration(
    const TEST_AD5940_ELECTROCHEMICAL_CALIBRATION_PARAMETERS *const parameters
)
{
	AD5940Err error = AD5940ERR_OK;

	test_ad5940_HstiaRtiaSel = parameters->HstiaRtiaSel;

	float Calibration_frequency;
	error = AD5940_get_calibration_frequency(
		test_ad5940_parameters_clockConfig.AdcClkFreq,
		UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_Calibration_frequency_Sample_period,
		UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftNum,
		UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc2Osr,
		UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc3Osr,
		&Calibration_frequency
	);
	if(error) return error;

	// cal ADC PGA
	ADCPGACal = (ADCPGACal_Type) {
		.AdcClkFreq = test_ad5940_parameters_clockConfig.AdcClkFreq,
		.ADCPga = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCPga,
		.ADCSinc2Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc2Osr,
		.ADCSinc3Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc3Osr,
		.PGACalType = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_PGACalType,
		.SysClkFreq = test_ad5940_parameters_clockConfig.SysClkFreq,
		.TimeOut10us = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_TimeOut10us,
		.VRef1p11 = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_VRef1p11,
		.VRef1p82 = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_VRef1p82,
	};
	error = AD5940_ADCPGACal(&ADCPGACal);
	if(error) return error;

	// cal LPTIA
	LPTIAOffsetCal = (LPTIAOffsetCal_Type) {
		.AdcClkFreq = test_ad5940_parameters_clockConfig.AdcClkFreq,
		.ADCPga = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCPga,
		.ADCSinc2Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc2Osr,
		.ADCSinc3Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc3Osr,
		.DacData12Bit = 0,
		.DacData6Bit = 0,
		.LpAmpPwrMod = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpAmpPwrMod,
		.LpAmpSel = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpAmpSel,
		.LpDacVzeroMux = LPDACVZERO_12BIT,
		.LpTiaRtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRtia,
		.LpTiaSW = LPTIASW(5) | LPTIASW(9),
		.SettleTime10us = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_SettleTime10us,
		.SysClkFreq = test_ad5940_parameters_clockConfig.SysClkFreq,
		.TimeOut10us = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_TimeOut10us,
	};
	error = AD5940_LPTIAOffsetCal(&LPTIAOffsetCal);
	if(error) return error;

	LPRTIACal = (LPRTIACal_Type) {
		.AdcClkFreq = test_ad5940_parameters_clockConfig.AdcClkFreq,
		.ADCSinc2Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc2Osr,
		.ADCSinc3Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc3Osr,
		.bPolarResult = bTRUE,
		.bWithCtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_bWithCtia,
		.DftCfg = {
			.DftNum = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftNum,
			.DftSrc = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftSrc,
			.HanWinEn = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HanWinEn,
		},
        .fFreq = Calibration_frequency,
		.fRcal = AD5940_EXTERNAL_fRcal,
		.LpAmpPwrMod = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpAmpPwrMod,
		.LpAmpSel = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpAmpSel,
		.LpTiaRtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRtia,
		.SysClkFreq = test_ad5940_parameters_clockConfig.SysClkFreq,
	};
	error = AD5940_LPRtiaCal(
		&LPRTIACal, 
		&test_ad5940_lprtia_result
	);
	test_ad5940_lprtia_result = (fImpPol_Type) {
		.Magnitude = 1e4,
		.Phase = 0,
	};
	if(error) return error;

	HSRTIACal = (HSRTIACal_Type) {
		.AdcClkFreq = test_ad5940_parameters_clockConfig.AdcClkFreq,
		.ADCSinc2Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc2Osr,
		.ADCSinc3Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc3Osr,
		.bPolarResult = bTRUE,
		.DftCfg = {
			.DftNum = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftNum,
			.DftSrc = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftSrc,
			.HanWinEn = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HanWinEn,
		},
        .fFreq = Calibration_frequency,
		.fRcal = AD5940_EXTERNAL_fRcal,
        .HsTiaCfg = (HSTIACfg_Type) {
            .DiodeClose = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DiodeClose,
            .ExtRtia = AD5940_EXTERNAL_HSRTIA,
            .HstiaCtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HstiaCtia,
            .HstiaDeRload = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HstiaDeRload,
            .HstiaDeRtia = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HstiaDeRtia,
            .HstiaRtiaSel = test_ad5940_HstiaRtiaSel,
        },
		.SysClkFreq = test_ad5940_parameters_clockConfig.SysClkFreq,
	};
	error = AD5940_HSRtiaCal(
		&HSRTIACal, 
		&test_ad5940_hsrtia_result
	);
	test_ad5940_hsrtia_result = (fImpPol_Type) {
		.Magnitude = 1e4,
		.Phase = 0,
	};
	if(error) return error;

    LPDACCal = (LPDACCal_Type) {
        .LpdacSel = LPAMP0,
        .ADCRefVolt = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADC_REFERENCE_VOLT,
        .SettleTime10us = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_SettleTime10us,
        .TimeOut10us = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_TimeOut10us,
        .AdcClkFreq = test_ad5940_parameters_clockConfig.AdcClkFreq,
        .SysClkFreq = test_ad5940_parameters_clockConfig.SysClkFreq,
        .ADCSinc2Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc2Osr,
        .ADCSinc3Osr = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc3Osr,
    };
    error = AD5940_LPDACCal(&LPDACCal, &test_ad5940_LPDAC_calibration_result);
    if(error != AD5940ERR_OK) return error;

	return error;
}
