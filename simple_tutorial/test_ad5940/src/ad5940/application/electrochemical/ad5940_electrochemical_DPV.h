#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_utility.h"
#include "ad5940_electrochemical_utility.h"

/**
 * @brief Options for signal inversion in Differential Pulse Voltammetry (DPV) measurements.
 * 
 * This enumeration defines the possible inversion options that can be applied during DPV measurements.
 */
typedef enum {
    AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_NONE,      /**< No inversion is applied. */
    AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_BOTH,      /**< Invert both anodic and cathodic signals. */
    AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_CATHODIC,  /**< Invert only the cathodic signal. */
    AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_ANODIC,    /**< Invert only the anodic signal. */
} AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION;

/**
 * @brief Macro to validate if a given inversion option is valid.
 * 
 * This macro checks if the provided option is one of the defined inversion options.
 * 
 * @param option The inversion option to validate.
 * 
 * @return Non-zero if the option is valid, zero otherwise.
 */
#define AD5940_ELECTROCHEMICAL_DPV_is_INVERSION_OPTION(option) (\
    (option == AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_NONE) || \
    (option == AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_BOTH) || \
    (option == AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_CATHODIC) || \
    (option == AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_ANODIC) \
)

/**
 * @brief Parameters for the AD5940 Electrochemical Differential Pulse Voltammetry (DPV) operation.
 */
typedef struct 
{
    int16_t E_begin;    /**< Starting potential of the scan, in millivolts (mV). */
    int16_t E_end;      /**< Ending potential of the scan, in millivolts (mV). */
    uint16_t E_step;    /**< Step potential between measurements, in millivolts (mV). */
    uint16_t E_pulse;   /**< Pulse amplitude, in millivolts (mV). */
    uint16_t t_pulse;   /**< Pulse duration, in milliseconds (ms). */
    uint16_t scan_rate; /**< Rate of potential change during the scan, in millivolts per second (mV/s). */
    AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION inversion_option; /**< Option to invert the signal during measurement. */
} 
AD5940_ELECTROCHEMICAL_DPV_PARAMETERS;

/**
 * @brief Generates time values for Differential Pulse Voltammetry (DPV) measurements.
 * 
 * This function calculates and populates an array of time values based on the provided
 * Differential Pulse Voltammetry (DPV) parameters and the specified number of scans.
 * 
 * @param parameters            Pointer to the structure containing DPV parameters.
 * @param times                 Pointer to an array where the calculated time values will be stored.
 * @param times_length          Pointer to a variable to store the actual number of generated time values.
 * @param times_max_length      The maximum capacity of the `times` array.
 * 
 * @return AD5940Err            Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_get_times(
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    uint32_t *const times,
    uint16_t *const times_length,
    const uint16_t times_max_length
);

/**
 * @brief Generates voltage values for Differential Pulse Voltammetry (DPV) measurements.
 * 
 * This function calculates and populates an array of voltage values based on the provided
 * Differential Pulse Voltammetry (DPV) parameters.
 * 
 * @param parameters            Pointer to the structure containing DPV parameters.
 * @param voltages              Pointer to an array where the calculated voltage values will be stored.
 * @param voltages_length       Pointer to a variable to store the actual number of generated voltage values.
 * @param voltages_max_length   The maximum capacity of the `voltages` array.
 * 
 * @return AD5940Err            Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_get_voltages(
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    int32_t *const voltages,
    uint16_t *const voltages_length,
    const uint16_t voltages_max_length
);

/**
 * @brief Configuration for Differential Pulse Voltammetry (DPV) using LPTIA.
 */
typedef struct 
{
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *parameters;     /**< Pointer to the DPV parameters. */

    float LFOSC_frequency;  /**< Low-frequency oscillator frequency, used for internal timing. 
                                 Obtainable via @ref AD5940_LFOSCMeasure in library/ad5940.h.*/
                                                        
    const AD5940_UTILITY_ClockConfig *clock;    /**< Pointer to clock configuration.
                                                     Obtainable via 
                                                     @ref AD5940_UTILITY_set_active_power 
                                                     in utility/ad5940_utility_power.h. */

    const AGPIOCfg_Type *agpio_cfg;     /**< Pointer to GPIO configuration. 
                                             - Refer to datasheet pages 112 and 122.
                                             - Configure GPIO for interrupts based on PCB design. */

    const AD5940_ELECTROCHEMICAL_UTILITY_AFERefCfg_Type *utility_AFERefCfg_Type;    /**< Pointer to AFE reference configuration. */
    const AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type *utility_LPPACfg_Type;        /**< Pointer to LPPA configuration. */
    const AD5940_ELECTROCHEMICAL_UTILITY_LPTIACfg_Type *utility_LPTIACfg_Type;      /**< Pointer to LPTIA configuration. */
    const AD5940_ELECTROCHEMICAL_UTILITY_DSPCfg_Type *utility_DSPCfg_Type;      /**< Pointer to DSP configuration. */

    uint32_t DataType; /**< Data type configuration. @ref DATATYPE_Const. */
    uint32_t FifoSrc;  /**< FIFO source configuration. @ref FIFOSRC_Const*/
} 
AD5940_ELECTROCHEMICAL_DPV_LPTIA_CONFIG;

/**
 * @brief Starts the Differential Pulse Voltammetry (DPV) operation using LPTIA configuration.
 * 
 * @param config Pointer to the LPTIA DPV configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_start_with_LPTIA(
    const AD5940_ELECTROCHEMICAL_DPV_LPTIA_CONFIG *const config
);

/**
 * @brief Configuration for Differential Pulse Voltammetry (DPV) with a specified working electrode.
 */
typedef struct 
{
    AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE working_electrode; /**< Type of working electrode. See @ref AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE. */
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *parameters;     /**< Pointer to the DPV parameters. */

    float LFOSC_frequency;  /**< Low-frequency oscillator frequency, used for internal timing. 
                                 Obtainable via @ref AD5940_LFOSCMeasure in library/ad5940.h.*/
                                                        
    const AD5940_UTILITY_ClockConfig *clock;    /**< Pointer to clock configuration.
                                                     Obtainable via 
                                                     @ref AD5940_UTILITY_set_active_power 
                                                     in utility/ad5940_utility_power.h. */

    const AGPIOCfg_Type *agpio_cfg;     /**< Pointer to GPIO configuration. 
                                             - Refer to datasheet pages 112 and 122.
                                             - Configure GPIO for interrupts based on PCB design. */

    const AD5940_ELECTROCHEMICAL_UTILITY_AFERefCfg_Type *utility_AFERefCfg_Type;    /**< Pointer to AFE reference configuration. */
    const AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type *utility_LPPACfg_Type;        /**< Pointer to LPPA configuration. */
    const AD5940_ELECTROCHEMICAL_UTILITY_HSTIACfg_Type *utility_HSTIACfg_Type;      /**< Pointer to HSTIA configuration. */
    const AD5940_ELECTROCHEMICAL_UTILITY_DSPCfg_Type *utility_DSPCfg_Type;      /**< Pointer to DSP configuration. */

    uint32_t DataType; /**< Data type configuration. @ref DATATYPE_Const. */
    uint32_t FifoSrc;  /**< FIFO source configuration. @ref FIFOSRC_Const*/
} 
AD5940_ELECTROCHEMICAL_DPV_CONFIG;

/**
 * @brief Starts the Differential Pulse Voltammetry (DPV) operation.
 * 
 * @param config Pointer to the DPV configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_start(
    const AD5940_ELECTROCHEMICAL_DPV_CONFIG *const config
);

/**
 * @brief Stops the Differential Pulse Voltammetry (DPV) operation and shuts down the AD5940.
 * 
 * @param MCU_FIFO_buffer            Pointer to the buffer where remaining FIFO data will be stored.
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param AD5940_FIFO_count          Pointer to a variable to retrieve the remaining FIFO count.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_stop(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count
);

/**
 * @brief Handles FIFO interrupts during Differential Pulse Voltammetry (DPV) operation.
 * 
 * @param MCU_FIFO_buffer            Pointer to the buffer to store FIFO data.
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param AD5940_FIFO_count          Pointer to retrieve the current FIFO count.
 * @param isContinue                 Flag to indicate whether to continue the CV operation.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_interrupt(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count
);

/**
 * @brief Converts ADC data to current values for Differential Pulse Voltammetry (DPV).
 * 
 * This function processes ADC data retrieved from the AD5940 and converts it to current 
 * values based on the provided calibration and configuration parameters. It is used in 
 * Differential Pulse Voltammetry (DPV) experiments.
 * 
 * @param currents              Pointer to an array where the calculated current values (in microamperes) will be stored.
 * @param currents_length       Pointer to a variable where the number of calculated current values will be stored.
 * @param currents_max_length   Maximum allowable length of the `currents` array..
 * @param adc_data              Pointer to the array of ADC data retrieved from the FIFO.
 * @param adc_data_length       Number of ADC data points in the `adc_data` array.
 * @param RtiaCalValue          Pointer to the RTIA calibration value. This parameter is obtained from RTIA calibration functions 
 *                              like @ref AD5940_HSRtiaCal or @ref AD5940_LPRtiaCal.
 * @param ADC_PGA_gain          ADC Programmable Gain Amplifier (PGA) gain value. Refer to @ref ADCPGA_Const.
 * @param ADC_reference_volt    Reference voltage used for the ADC (in volts). Refer to the AD5940 datasheet for details.
 *
 * @return AD5940Err            Returns an error code of type `AD5940Err`. A value of 0 indicates success, while other values 
 *                              indicate specific errors encountered during the conversion process.
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_convert_ADCs_to_currents(
    int32_t *const currents, 
    uint16_t *const currents_length,
    const uint16_t currents_max_length,
    const uint32_t *const adc_data, 
    const uint16_t adc_data_length, 
    const fImpPol_Type *const RtiaCalValue, 
    const uint32_t ADC_PGA_gain, 
    const float ADC_reference_volt
);

#ifdef __cplusplus
}
#endif
