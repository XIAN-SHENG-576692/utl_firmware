#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_utility.h"
#include "ad5940_electrochemical_utility.h"

/**
 * @brief Parameters for the AD5940 Electrochemical Cyclic Voltammetry (CV) operation.
 */
typedef struct 
{
    int16_t E_begin;      /**< Starting potential of the scan, in millivolts (mV). */
    int16_t E_vertex1;    /**< First vertex potential of the scan, in millivolts (mV). */
    int16_t E_vertex2;    /**< Second vertex potential of the scan, in millivolts (mV). */
    uint16_t E_step;      /**< Step potential between measurements, in millivolts (mV). */
    uint16_t scan_rate;   /**< Rate of potential change during the scan, in millivolts per second (mV/s). */
} 
AD5940_ELECTROCHEMICAL_CV_PARAMETERS;

/**
 * @brief Generates time values for Cyclic Voltammetry (CV) measurements.
 * 
 * This function calculates and populates an array of time values based on the provided
 * Cyclic Voltammetry (CV) parameters and the specified number of scans.
 * 
 * @param parameters            Pointer to the structure containing CV parameters.
 * @param number_of_scans       The number of scans for which time values need to be generated.
 * @param times                 Pointer to an array where the calculated time values will be stored.
 * @param times_length          Pointer to a variable to store the actual number of generated time values.
 * @param times_max_length      The maximum capacity of the `times` array.
 * 
 * @return AD5940Err            Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_get_times(
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint16_t number_of_scans,
    uint32_t *const times,
    uint16_t *const times_length,
    const uint16_t times_max_length
);

/**
 * @brief Generates voltage values for Cyclic Voltammetry (CV) measurements.
 * 
 * This function calculates and populates an array of voltage values based on the provided
 * Cyclic Voltammetry (CV) parameters and the specified number of scans.
 * 
 * @param parameters            Pointer to the structure containing CV parameters.
 * @param number_of_scans       The number of scans for which voltage values need to be generated.
 * @param voltages              Pointer to an array where the calculated voltage values will be stored.
 * @param voltages_length       Pointer to a variable to store the actual number of generated voltage values.
 * @param voltages_max_length   The maximum capacity of the `voltages` array.
 * 
 * @return AD5940Err            Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_get_voltages(
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint16_t number_of_scans,
    int32_t *const voltages,
    uint16_t *const voltages_length,
    const uint16_t voltages_max_length
);

/**
 * @brief Configuration for Cyclic Voltammetry (CV) using LPTIA.
 */
typedef struct 
{
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *parameters;     /**< Pointer to the CV parameters. */

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
AD5940_ELECTROCHEMICAL_CV_LPTIA_CONFIG;

/**
 * @brief Starts the Cyclic Voltammetry (CV) operation using LPTIA configuration.
 * 
 * @param config Pointer to the LPTIA CV configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_start_with_LPTIA(
    const AD5940_ELECTROCHEMICAL_CV_LPTIA_CONFIG *const config
);

/**
 * @brief Configuration for Cyclic Voltammetry (CV) with a specified working electrode.
 */
typedef struct 
{
    AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE working_electrode; /**< Type of working electrode. See @ref AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE. */
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *parameters;     /**< Pointer to the CV parameters. */

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
AD5940_ELECTROCHEMICAL_CV_CONFIG;

/**
 * @brief Starts the Cyclic Voltammetry (CV) operation.
 * 
 * @param config Pointer to the CV configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_start(
    const AD5940_ELECTROCHEMICAL_CV_CONFIG *const config
);

/**
 * @brief Stops the Cyclic Voltammetry (CV) operation and shuts down the AD5940.
 * 
 * @param MCU_FIFO_buffer            Pointer to the buffer where remaining FIFO data will be stored.
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param AD5940_FIFO_count          Pointer to a variable to retrieve the remaining FIFO count.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_stop(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count
);

/**
 * @brief Handles FIFO interrupts during Cyclic Voltammetry (CV) operation.
 * 
 * @param MCU_FIFO_buffer            Pointer to the buffer to store FIFO data.
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param AD5940_FIFO_count          Pointer to retrieve the current FIFO count.
 * @param isContinue                 Flag to indicate whether to continue the CV operation.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_interrupt(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count,
    const BoolFlag isContinue
);

#ifdef __cplusplus
}
#endif
