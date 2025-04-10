#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"

#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_SettleTime10us 1E3
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_TimeOut10us 1E3

#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCPga ADCPGA_1
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCAvgNum ADCAVGNUM_16

/**
 * Refer to page 57 of the datasheet for more details.
 */
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc2Osr ADCSINC2OSR_22

/**
 * Refer to page 57 of the datasheet.
 * ```
 * It is recommended to use a decimation rate of 4.
 * ```
 */
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCSinc3Osr ADCSINC3OSR_4

#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_BpNotch bTRUE
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_BpSinc3 bFALSE
// #define _ELECTROCHEMICALDFTClkEnable
// #define _ELECTROCHEMICALSinc2NotchClkEnable,
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_Sinc2NotchEnable bTRUE

#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpAmpPwrMod LPAMPPWR_NORM
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpAmpSel LPAMP0
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRtia LPTIARTIA_10K
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRf LPTIARF_SHORT
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_LpTiaRload LPTIARLOAD_10R

#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_PGACalType PGACALTYPE_OFFSETGAIN
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_VRef1p11 1.11F

/**
 * Refer to pgae 87 of the datasheet.
 * This application use internal reference voltage.
 */
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_VRef1p82 1.82F
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADC_REFERENCE_VOLT UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_VRef1p82

#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftNum DFTNUM_16
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DftSrc DFTSRC_SINC2NOTCH
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HanWinEn bTRUE

#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_bWithCtia bTRUE

/* Sample 3 period of signal, 13.317Hz here. Do not use DC method, because it needs ADC/PGA calibrated firstly(but it's faster) */
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_Calibration_frequency_Sample_period 3

/**
 * Refer to page 52 of the datasheet.
 * 31pF + 2pF
 */
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HstiaCtia 31

/**
 * Refer to Figure 28 (page 50) and Figure 36 (page 76) of the datasheet.
 * To streamline the working electrode configuration by ensuring all electrode_routing use the same RTIA value, ignore HstiaDeRload.
 */
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HstiaDeRload HSTIADERLOAD_0R

/**
 * Refer to Figure 28 (page 50) and Figure 36 (page 76) of the datasheet.
 * To streamline the working electrode configuration by ensuring all electrode_routing use the same RTIA value, ignore HstiaDeRtia.
 */
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_HstiaDeRtia HSTIADERTIA_OPEN

// This parameter is chosen arbitrarily.
#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DiodeClose bFALSE

#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DataType DATATYPE_SINC2

#define UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_FifoSrc FIFOSRC_SINC2NOTCH

#ifdef __cplusplus
}
#endif
