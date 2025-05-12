#include "ad5940_electrochemical_calibration.h"

#include "ad5940_hardware.h"

AD5940Err ad5940_electrochemical_calibration(
    const AD5940_ELECTROCHEMICAL_CALIBRATION_PARAMETERS *const parameters,
    AD5940_ELECTROCHEMICAL_CALIBRATION_RESULTS *const results
)
{
	AD5940Err error = AD5940ERR_OK;

	error = AD5940_get_calibration_frequency(
		parameters->clockConfig.AdcClkFreq,
		parameters->SamplePeriod,
		parameters->DftNum,
		parameters->ADCSinc2Osr,
		parameters->ADCSinc3Osr,
		&results->CalibrationFrequency
	);
	if(error) return error;

	// cal ADC PGA
	{
		ADCPGACal_Type ADCPGACal;
		ADCPGACal = (ADCPGACal_Type) {
			.AdcClkFreq = parameters->clockConfig.AdcClkFreq,
			.ADCPga = parameters->ADCPga,
			.ADCSinc2Osr = parameters->ADCSinc2Osr,
			.ADCSinc3Osr = parameters->ADCSinc3Osr,
			.PGACalType = parameters->PGACalType,
			.SysClkFreq = parameters->clockConfig.SysClkFreq,
			.TimeOut10us = parameters->TimeOut10us,
			.VRef1p11 = parameters->VRef1p11,
			.VRef1p82 = parameters->VRef1p82,
		};
		error = AD5940_ADCPGACal(&ADCPGACal);
		if(error) return error;
	}

	// // cal LPTIA
	// {
	// 	LPTIAOffsetCal_Type LPTIAOffsetCal;
	// 	LPTIAOffsetCal = (LPTIAOffsetCal_Type) {
	// 		.AdcClkFreq = parameters->clockConfig.AdcClkFreq,
	// 		.ADCPga = parameters->ADCPga,
	// 		.ADCSinc2Osr = parameters->ADCSinc2Osr,
	// 		.ADCSinc3Osr = parameters->ADCSinc3Osr,
	// 		.DacData12Bit = 0,
	// 		.DacData6Bit = 0,
	// 		.LpAmpPwrMod = parameters->LpAmpPwrMod,
	// 		.LpAmpSel = parameters->LpAmpSel,
	// 		.LpDacVzeroMux = LPDACVZERO_12BIT,
	// 		.LpTiaRtia = parameters->LpTiaRtia,
	// 		.LpTiaSW = LPTIASW(5) | LPTIASW(9),
	// 		.SettleTime10us = parameters->SettleTime10us,
	// 		.SysClkFreq = parameters->clockConfig.SysClkFreq,
	// 		.TimeOut10us = parameters->TimeOut10us,
	// 	};
	// 	error = AD5940_LPTIAOffsetCal(&LPTIAOffsetCal);
	// 	if(error) return error;
	// }

	{
		LPRTIACal_Type LPRTIACal;
		LPRTIACal = (LPRTIACal_Type) {
			.AdcClkFreq = parameters->clockConfig.AdcClkFreq,
			.ADCSinc2Osr = parameters->ADCSinc2Osr,
			.ADCSinc3Osr = parameters->ADCSinc3Osr,
			.bPolarResult = bTRUE,
			.bWithCtia = parameters->bWithCtia,
			.DftCfg = {
				.DftNum = parameters->DftNum,
				.DftSrc = parameters->DftSrc,
				.HanWinEn = parameters->HanWinEn,
			},
			.fFreq = results->CalibrationFrequency,
			.fRcal = AD5940_EXTERNAL_fRcal,
			.LpAmpPwrMod = parameters->LpAmpPwrMod,
			.LpAmpSel = parameters->LpAmpSel,
			.LpTiaRtia = parameters->LpTiaRtia,
			.SysClkFreq = parameters->clockConfig.SysClkFreq,
		};
		error = AD5940_LPRtiaCal(
			&LPRTIACal, 
			&results->lprtia_calibration_result
		);
		results->lprtia_calibration_result = (fImpPol_Type) {
			.Magnitude = 1e4,
			.Phase = 0,
		};
		if(error) return error;
	}

	{
		HSRTIACal_Type HSRTIACal;
		HSRTIACal = (HSRTIACal_Type) {
			.AdcClkFreq = parameters->clockConfig.AdcClkFreq,
			.ADCSinc2Osr = parameters->ADCSinc2Osr,
			.ADCSinc3Osr = parameters->ADCSinc3Osr,
			.bPolarResult = bTRUE,
			.DftCfg = {
				.DftNum = parameters->DftNum,
				.DftSrc = parameters->DftSrc,
				.HanWinEn = parameters->HanWinEn,
			},
			.fFreq = results->CalibrationFrequency,
			.fRcal = AD5940_EXTERNAL_fRcal,
			.HsTiaCfg = (HSTIACfg_Type) {
				.DiodeClose = parameters->DiodeClose,
				.ExtRtia = AD5940_EXTERNAL_HSRTIA,
				.HstiaCtia = parameters->HstiaCtia,
				.HstiaDeRload = parameters->HstiaDeRload,
				.HstiaDeRtia = parameters->HstiaDeRtia,
				.HstiaRtiaSel = parameters->HstiaRtiaSel,
			},
			.SysClkFreq = parameters->clockConfig.SysClkFreq,
		};
		error = AD5940_HSRtiaCal(
			&HSRTIACal, 
			&results->hsrtia_calibration_result
		);
		results->hsrtia_calibration_result = (fImpPol_Type) {
			.Magnitude = 1e4,
			.Phase = 0,
		};
		if(error) return error;
	}

	{
		LPDACCal_Type LPDACCal;
		LPDACCal = (LPDACCal_Type) {
			.LpdacSel = LPAMP0,
			.ADCRefVolt = parameters->ADCRefVolt,
			.SettleTime10us = parameters->SettleTime10us,
			.TimeOut10us = parameters->TimeOut10us,
			.AdcClkFreq = parameters->clockConfig.AdcClkFreq,
			.SysClkFreq = parameters->clockConfig.SysClkFreq,
			.ADCSinc2Osr = parameters->ADCSinc2Osr,
			.ADCSinc3Osr = parameters->ADCSinc3Osr,
		};
		error = AD5940_LPDACCal(&LPDACCal, &results->lpdac_calibration_result);
		if(error != AD5940ERR_OK) return error;
	}

	return error;
}
