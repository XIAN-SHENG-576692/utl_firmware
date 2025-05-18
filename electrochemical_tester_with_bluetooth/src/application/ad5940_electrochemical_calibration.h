#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_electrochemical_utils.h"

typedef struct
{
    AD5940_ClockConfig clockConfig;
    float lfoscFrequency;

    /**
     * Refer to pages 50 and 52 of the datasheet.
     * @ref HSTIARTIA_Const
     */
    uint32_t HstiaRtiaSel;

    uint32_t SamplePeriod;
    
    float ADCRefVolt;
    int32_t SettleTime10us;
    int32_t TimeOut10us;
    
    uint32_t ADCSinc2Osr;
    uint32_t ADCSinc3Osr;

    uint32_t ADCPga;
    uint32_t PGACalType;
    float VRef1p11;
    float VRef1p82;
    
    uint32_t DftNum;
    uint32_t DftSrc;

    BoolFlag bWithCtia;
    BoolFlag HanWinEn;
    
    BoolFlag DiodeClose;

    uint32_t HstiaCtia;
    uint32_t HstiaDeRload;
    uint32_t HstiaDeRtia;

    uint32_t LpAmpPwrMod;
    uint32_t LpAmpSel;
    uint32_t LpTiaRtia;
    
} AD5940_ELECTROCHEMICAL_CALIBRATION_PARAMETERS;

typedef struct
{
    fImpPol_Type lprtia_calibration_result;
    fImpPol_Type hsrtia_calibration_result;
    LPDACPara_Type lpdac_calibration_result;
	float CalibrationFrequency;
} AD5940_ELECTROCHEMICAL_CALIBRATION_RESULTS;

AD5940Err ad5940_electrochemical_calibration(
    const AD5940_ELECTROCHEMICAL_CALIBRATION_PARAMETERS *const parameters,
    AD5940_ELECTROCHEMICAL_CALIBRATION_RESULTS *const results
);

#ifdef __cplusplus
}
#endif
