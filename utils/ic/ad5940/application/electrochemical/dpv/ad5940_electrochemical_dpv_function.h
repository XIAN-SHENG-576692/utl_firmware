#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940_electrochemical_dpv_struct.h"

/**
 * @brief Configuration for Differential Pulse Voltammetry (DPV) using LPTIA.
 */
typedef struct 
{
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *parameters;     /**< Pointer to the DPV parameters. */

    float LFOSC_frequency;  /**< Low-frequency oscillator frequency, used for internal timing. 
                                 Obtainable via @ref AD5940_LFOSCMeasure in library/ad5940.h.*/
                                                        
    const AD5940_ClockConfig *clock;    /**< Pointer to clock configuration.
                                                     Obtainable via 
                                                     @ref AD5940_set_active_power 
                                                     in utility/ad5940_utility_power.h. */

    const AGPIOCfg_Type *agpio_cfg;     /**< Pointer to GPIO configuration. 
                                             - Refer to datasheet pages 112 and 122.
                                             - Configure GPIO for interrupts based on PCB design. */

    const AD5940_ELECTROCHEMICAL_AFERefCfg_Type *afe_ref_cfg;    /**< Pointer to AFE reference configuration. */
    const AD5940_ELECTROCHEMICAL_LPDACfg_Type *lpdac_cfg;        /**< Pointer to LPPA configuration. */
    const AD5940_ELECTROCHEMICAL_LPTIACfg_Type *lptia_cfg;      /**< Pointer to LPTIA configuration. */
    const AD5940_ELECTROCHEMICAL_DSPCfg_Type *dsp_cfg;      /**< Pointer to DSP configuration. */

    uint32_t DataType; /**< Data type configuration. @ref DATATYPE_Const. */
    uint32_t FifoSrc;  /**< FIFO source configuration. @ref FIFOSRC_Const*/
} 
AD5940_ELECTROCHEMICAL_DPV_LPDAC_LPTIA_CONFIG;

/**
 * @brief Starts the Differential Pulse Voltammetry (DPV) operation using LPTIA configuration.
 * 
 * @param config Pointer to the LPTIA DPV configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_start_with_LPDAC_LPTIA(
    const AD5940_ELECTROCHEMICAL_DPV_LPDAC_LPTIA_CONFIG *const config
);

/**
 * @brief Configuration for Differential Pulse Voltammetry (DPV) with a specified working electrode.
 */
typedef struct 
{
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *parameters;     /**< Pointer to the DPV parameters. */

    const AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING *electrode_routing;

    float LFOSC_frequency;  /**< Low-frequency oscillator frequency, used for internal timing. 
                                 Obtainable via @ref AD5940_LFOSCMeasure in library/ad5940.h.*/
                                                        
    const AD5940_ClockConfig *clock;    /**< Pointer to clock configuration.
                                                     Obtainable via 
                                                     @ref AD5940_set_active_power 
                                                     in utility/ad5940_utility_power.h. */

    const AGPIOCfg_Type *agpio_cfg;     /**< Pointer to GPIO configuration. 
                                             - Refer to datasheet pages 112 and 122.
                                             - Configure GPIO for interrupts based on PCB design. */

    const AD5940_ELECTROCHEMICAL_AFERefCfg_Type *afe_ref_cfg;    /**< Pointer to AFE reference configuration. */
    const AD5940_ELECTROCHEMICAL_LPDACfg_Type *lpdac_cfg;        /**< Pointer to LPPA configuration. */
    const AD5940_ELECTROCHEMICAL_HSTIACfg_Type *hstia_cfg;      /**< Pointer to HSTIA configuration. */
    const AD5940_ELECTROCHEMICAL_DSPCfg_Type *dsp_cfg;      /**< Pointer to DSP configuration. */

    uint32_t DataType; /**< Data type configuration. @ref DATATYPE_Const. */
    uint32_t FifoSrc;  /**< FIFO source configuration. @ref FIFOSRC_Const*/
} 
AD5940_ELECTROCHEMICAL_DPV_LPDAC_HSTIA_CONFIG;

/**
 * @brief Starts the Differential Pulse Voltammetry (DPV) operation.
 * 
 * @param config Pointer to the DPV configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_start_with_LPDAC_HSTIA(
    const AD5940_ELECTROCHEMICAL_DPV_LPDAC_HSTIA_CONFIG *const config
);

/**
 * @brief Stops the Differential Pulse Voltammetry (DPV) operation and shuts down the AD5940.
 * 
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param MCU_FIFO_buffer            Pointer to the buffer where remaining FIFO data will be stored.
 * @param MCU_FIFO_count             Pointer to a variable to retrieve the remaining FIFO count.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_stop(
    const uint16_t MCU_FIFO_buffer_max_length,
    uint32_t *const MCU_FIFO_buffer, 
    uint16_t *const MCU_FIFO_count
);

/**
 * @brief Handles FIFO interrupts during Differential Pulse Voltammetry (DPV) operation.
 * 
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param MCU_FIFO_buffer            Pointer to the buffer to store FIFO data.
 * @param MCU_FIFO_count             Pointer to retrieve the current FIFO count.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_interrupt(
    const uint16_t MCU_FIFO_buffer_max_length,
    uint32_t *const MCU_FIFO_buffer, 
    uint16_t *const MCU_FIFO_count
);

/**
 * @brief Converts ADC data to current values for Differential Pulse Voltammetry (DPV).
 * 
 * This function processes ADC data retrieved from the AD5940 and converts it to current 
 * values based on the provided calibration and configuration parameters. It is used in 
 * Differential Pulse Voltammetry (DPV) experiments.
 * 
 * @param currents_max_length   Maximum allowable length of the `currents` array..
 * @param adc_data              Pointer to the array of ADC data retrieved from the FIFO.
 * @param adc_data_length       Number of ADC data points in the `adc_data` array.
 * @param RtiaCalValue          Pointer to the RTIA calibration value. This parameter is obtained from RTIA calibration functions 
 *                              like @ref AD5940_HSRtiaCal or @ref AD5940_LPRtiaCal.
 * @param ADC_PGA_gain          ADC Programmable Gain Amplifier (PGA) gain value. Refer to @ref ADCPGA_Const.
 * @param ADC_reference_volt    Reference voltage used for the ADC (in volts). Refer to the AD5940 datasheet for details.
 * @param currents              Pointer to an array where the calculated current values (in microamperes) will be stored.
 * @param currents_length       Pointer to a variable where the number of calculated current values will be stored.
 *
 * @return AD5940Err            Returns an error code of type `AD5940Err`. A value of 0 indicates success, while other values 
 *                              indicate specific errors encountered during the conversion process.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_convert_ADCs_to_currents(
    const uint16_t currents_max_length,
    const uint32_t *const adc_data, 
    const uint16_t adc_data_length, 
    const fImpPol_Type *const RtiaCalValue, 
    const uint32_t ADC_PGA_gain, 
    const float ADC_reference_volt,
    int32_t *const currents, 
    uint16_t *const currents_length
);

#ifdef __cplusplus
}
#endif
