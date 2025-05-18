#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"

#define UTL_AD5940_TEMPERATURE_PARAMETERS_ADCPga ADCPGA_1P5

#define UTL_AD5940_TEMPERATURE_PARAMETERS_FIFO_thresh 1
#define UTL_AD5940_TEMPERATURE_PARAMETERS_ADC_sample_interval 250

#define UTL_AD5940_TEMPERATURE_PARAMETERS_ADCSinc2Osr ADCSINC2OSR_22
#define UTL_AD5940_TEMPERATURE_PARAMETERS_ADCSinc3Osr ADCSINC3OSR_4

#define UTL_AD5940_TEMPERATURE_PARAMETERS_FifoSrc FIFOSRC_SINC2NOTCH
#define UTL_AD5940_TEMPERATURE_PARAMETERS_DataType DATATYPE_NOTCH
#define UTL_AD5940_TEMPERATURE_PARAMETERS_ADCAvgNum ADCAVGNUM_2

#define UTL_AD5940_TEMPERATURE_PARAMETERS_BpNotch bTRUE                 /* SINC2+Notch is one block, when bypass notch filter, we can get fresh data from SINC2 filter. */
#define UTL_AD5940_TEMPERATURE_PARAMETERS_BpSinc3 bFALSE                /* We use SINC3 filter. */
#define UTL_AD5940_TEMPERATURE_PARAMETERS_Sinc2NotchEnable bTRUE        /* Enable the SINC2+Notch block. You can also use function AD5940_AFECtrlS */

#define UTL_AD5940_TEMPERATURE_PARAMETERS_TEMPSENS 0x0000

#ifdef __cplusplus
}
#endif
