#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"

/**
 * Calculates the calibration frequency based on various parameters.
 * 
 * @param Calibration_frequency Pointer to store the calculated calibration frequency (in Hz).
 * @param ADC_CLOCK_FREQUENCY   The clock frequency of the ADC (in Hz).
 * @param Sample_period         Sampling period.
 * @param DFTNUM_Const          DFT (Discrete Fourier Transform) configuration constant.
 * @param ADCSINC2OSR_Const     ADC SINC2 oversampling rate constant.
 * @param ADCSINC3OSR_Const     ADC SINC3 oversampling rate constant.
 * 
 * @return AD5940Err Error code indicating the success or failure of the operation.
 */
AD5940Err AD5940_UTILITY_get_Calibration_frequency(
    float *const Calibration_frequency,
    const float ADC_CLOCK_FREQUENCY,
    const uint32_t Sample_period,
    const uint32_t DFTNUM_Const,
    const uint32_t ADCSINC2OSR_Const,
    const uint32_t ADCSINC3OSR_Const
);

/**
 * Retrieves the DFTNUM value based on the DFTNUM constant.
 * 
 * @param DFTNUM_Const The DFTNUM configuration constant.
 * @param DFTNUM       Pointer to store the resulting DFTNUM value.
 * 
 * @return AD5940Err Error code indicating the success or failure of the operation.
 */
AD5940Err AD5940_UTILITY_get_DFTNUM(
    const uint32_t DFTNUM_Const, 
    uint16_t *const DFTNUM
);

/**
 * Retrieves the ADC SINC2 oversampling rate based on the given constant.
 * 
 * @param ADCSINC2OSR_Const The ADC SINC2 oversampling rate constant.
 * @param ADCSinc2Osr       Pointer to store the resulting oversampling rate.
 * 
 * @return AD5940Err Error code indicating the success or failure of the operation.
 */
AD5940Err AD5940_UTILITY_get_ADCSinc2Osr(
    const uint32_t ADCSINC2OSR_Const, 
    uint16_t *const ADCSinc2Osr
);

/**
 * Retrieves the ADC SINC3 oversampling rate based on the given constant.
 * 
 * @param ADCSINC3OSR_Const The ADC SINC3 oversampling rate constant.
 * @param ADCSinc3Osr       Pointer to store the resulting oversampling rate.
 * 
 * @return AD5940Err Error code indicating the success or failure of the operation.
 */
AD5940Err AD5940_UTILITY_get_ADCSinc3Osr(
    const uint32_t ADCSINC3OSR_Const, 
    uint16_t *const ADCSinc3Osr
);

/**
 * Retrieves the ADC Programmable Gain Amplifier (PGA) value.
 * 
 * @note Refer to pages 54 to 57 of the datasheet for more details.
 * 
 * @param ADCPGA_Const ADC PGA configuration constant. See @ref ADCPGA_Const.
 * @param ADCPGA       Pointer to store the calculated PGA value (in volts).
 * 
 * @return AD5940Err Error code
 */
AD5940Err AD5940_UTILITY_get_ADCPGA(
    const uint32_t ADCPGA_Const, 
    float *const ADCPGA
);

/**
 * Converts ADC data to current values.
 * 
 * @param currents              Pointer to store the resulting current values (in microamperes).
 * @param adc_data              Pointer to the ADC data retrieved from the FIFO.
 * @param adc_data_length       Number of ADC data points.
 * @param RtiaCalValue          Pointer to the RTIA calibration result. Available after calibration:
 *                              - @ref AD5940_HSRtiaCal
 *                              - @ref AD5940_LPRtiaCal
 * @param ADC_PGA_gain          ADC PGA gain value. See @ref ADCPGA_Const.
 * @param ADC_reference_volt    Reference voltage for the ADC (in volts). Refer to datasheet page 87.
 * 
 * @return AD5940Err Error code indicating the success or failure of the operation.
 */
AD5940Err AD5940_UTILITY_convert_ADCs_to_currents(
    int32_t *const currents, 
    const uint32_t *const adc_data, 
    const uint16_t adc_data_length, 
    const fImpPol_Type *const RtiaCalValue,
    const uint32_t ADC_PGA_gain,
    const float ADC_reference_volt
);

/**
 * Converts ADC data to temperature values.
 * 
 * @param temperatures          Pointer to store the resulting temperature values (in microdegrees Celsius).
 * @param adc_data              Pointer to the ADC data retrieved from the FIFO.
 * @param adc_data_length       Number of ADC data points.
 * @param ADCPGA_Const          ADC PGA gain configuration constant. See @ref ADCPGA_Const.
 * 
 * @return AD5940Err Error code indicating the success or failure of the operation.
 */
AD5940Err AD5940_UTILITY_convert_ADCs_to_temperatures(
    int32_t *const temperatures, 
    const uint32_t *const adc_data, 
    const uint16_t adc_data_length,
    const uint32_t ADCPGA_Const
);

#ifdef __cplusplus
}
#endif
