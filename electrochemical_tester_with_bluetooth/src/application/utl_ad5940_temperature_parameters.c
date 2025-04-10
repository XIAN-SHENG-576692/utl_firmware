#include "utl_ad5940_temperature_parameters.h"

#include "ad5940_temperature.h"

#define _TEMPERATURE_FIFO_thresh 1
#define _TEMPERATURE_ADC_sample_interval 250

#define _TEMPERATURE_ADCSinc2Osr ADCSINC2OSR_22
#define _TEMPERATURE_ADCSinc3Osr ADCSINC3OSR_4

#define _TEMPERATURE_FifoSrc FIFOSRC_SINC2NOTCH
#define _TEMPERATURE_DataType DATATYPE_NOTCH
#define _TEMPERATURE_ADCAvgNum ADCAVGNUM_2

#define _TEMPERATURE_BpNotch bTRUE                 /* SINC2+Notch is one block, when bypass notch filter, we can get fresh data from SINC2 filter. */
#define _TEMPERATURE_BpSinc3 bFALSE                /* We use SINC3 filter. */
#define _TEMPERATURE_Sinc2NotchEnable bTRUE        /* Enable the SINC2+Notch block. You can also use function AD5940_AFECtrlS */

#define _TEMPERATURE_TEMPSENS 0x0000
// #define _TEMPERATURE_TEMPSENS 0x1111

static AD5940_TEMPERATURE_ANALOG_CONFIG _analog_config = {
	.ADCAvgNum = _TEMPERATURE_ADCAvgNum,
	.ADCPga = TEST_AD5940_TEMPERATURE_ADCPga,
	.ADCSinc2Osr = _TEMPERATURE_ADCSinc2Osr,
	.ADCSinc3Osr = _TEMPERATURE_ADCSinc3Osr,
	.BpNotch = _TEMPERATURE_BpNotch,
	.BpSinc3 = _TEMPERATURE_BpSinc3,
	.DataType = _TEMPERATURE_DataType,
	.FifoSrc = _TEMPERATURE_FifoSrc,
	.Sinc2NotchEnable = _TEMPERATURE_Sinc2NotchEnable,
};

/**
 * You need to set the following parameters:
 * - agpio_cfg
 * - clock
 * - LFOSC_frequency
 */
AD5940_TEMPERATURE_START_CONFIG utl_ad5940_temperature_start_config = {
	.ADC_sample_interval = _TEMPERATURE_ADC_sample_interval,
	// .agpio_cfg = &agpio_cfg,
	// .clock = &clockConfig,
	.analog = &_analog_config,
	.FIFO_thresh = _TEMPERATURE_FIFO_thresh,
	// .LFOSC_frequency = LFOSC_frequency,
	.TEMPSENS = _TEMPERATURE_TEMPSENS,
};
