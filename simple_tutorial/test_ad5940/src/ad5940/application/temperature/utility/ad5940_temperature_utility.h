#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_utility.h"

/**
 * Configuration structure for temperature measurement using the AD5940.
 */
typedef struct
{
    uint32_t ADCSinc2Osr;       /**< ADC SINC2 oversampling rate. Refer to @ref ADCSINC2OSR_Const. */
    uint32_t ADCSinc3Osr;       /**< ADC SINC3 oversampling rate. Refer to @ref ADCSINC3OSR_Const. */
    uint32_t ADCAvgNum;         /**< ADC averaging number. Refer to @ref ADCAVGNUM_Const. */
    uint32_t ADCPga;            /**< ADC Programmable Gain Amplifier (PGA) setting. Refer to @ref ADCPGA_Const. */

    uint32_t DataType;          /**< Data type configuration. Refer to @ref DATATYPE_Const. */
    uint32_t FifoSrc;           /**< FIFO source configuration. Refer to @ref FIFOSRC_Const. */

    BoolFlag BpNotch;           /**< Enable or disable bypass for the notch filter. */
    BoolFlag BpSinc3;           /**< Enable or disable bypass for the SINC3 filter. */
    BoolFlag Sinc2NotchEnable;  /**< Enable or disable the SINC2-notch filter. */
}
AD5940_TEMPERATURE_ANALOG_CONFIG;

#ifdef __cplusplus
}
#endif
