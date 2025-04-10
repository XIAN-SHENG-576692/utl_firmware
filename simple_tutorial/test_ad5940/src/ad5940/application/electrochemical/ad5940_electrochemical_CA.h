#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_utility.h"
#include "ad5940_electrochemical_utility.h"

/**
 * @brief Parameters for the AD5940 Electrochemical Chronoamperometry (CA) operation.
 */
typedef struct 
{
    int16_t E_dc;           /**< DC potential applied during the experiment in millivolts (mV). */
    uint16_t t_interval;    /**< Time interval between measurements in milliseconds (ms). */
    uint32_t t_run;         /**< Total runtime for the experiment in milliseconds (ms). */
} 
AD5940_ELECTROCHEMICAL_CA_PARAMETERS;

/**
 * @brief Calculates the time points for Chronoamperometry (CA) measurements.
 * 
 * This function generates an array of time points based on the provided CA parameters.
 * 
 * @param parameters            Pointer to the CA parameters structure.
 * @param times                 Pointer to an array to store calculated time points.
 * @param times_length          Pointer to a variable to store the number of generated time points.
 * @param times_max_length      Maximum allowable length of the `times` array.
 *
 * @return AD5940Err            Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_get_times(
    const AD5940_ELECTROCHEMICAL_CA_PARAMETERS *const parameters,
    uint32_t *const times,
    uint16_t *const times_length,
    const uint16_t times_max_length
);

/**
 * @brief Calculates the voltage values for Chronoamperometry (CA) measurements.
 * 
 * This function generates an array of voltage values based on the provided CA parameters.
 * 
 * @param parameters            Pointer to the CA parameters structure.
 * @param voltages              Pointer to an array to store calculated voltages.
 * @param voltages_length       Pointer to a variable to store the number of generated voltages.
 * @param voltages_max_length   Maximum allowable length of the `voltages` array.
 * 
 * @return AD5940Err            Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_get_voltages(
    const AD5940_ELECTROCHEMICAL_CA_PARAMETERS *const parameters,
    int32_t *const voltages,
    uint16_t *const voltages_length,
    const uint16_t voltages_max_length
);

/**
 * @brief Configuration for Chronoamperometry (CA) using LPTIA.
 */
typedef struct 
{
    const AD5940_ELECTROCHEMICAL_CA_PARAMETERS *parameters;     /**< Pointer to the CA parameters. */

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
AD5940_ELECTROCHEMICAL_CA_LPTIA_CONFIG;

/**
 * @brief Starts the Chronoamperometry (CA) operation using LPTIA configuration.
 * 
 * @param config Pointer to the LPTIA CA configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_start_with_LPTIA(
    const AD5940_ELECTROCHEMICAL_CA_LPTIA_CONFIG *const config
);

/**
 * @brief Configuration for Chronoamperometry (CA) with a specified working electrode.
 */
typedef struct 
{
    AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE working_electrode; /**< Type of working electrode. See @ref AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE. */
    const AD5940_ELECTROCHEMICAL_CA_PARAMETERS *parameters;     /**< Pointer to the CA parameters. */

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
AD5940_ELECTROCHEMICAL_CA_CONFIG;

/**
 * @brief Starts the Chronoamperometry (CA) operation.
 * 
 * @param config Pointer to the CA configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_start(
    const AD5940_ELECTROCHEMICAL_CA_CONFIG *const config
);

/**
 * @brief Stops the Chronoamperometry (CA) operation and shuts down the AD5940.
 * 
 * @param MCU_FIFO_buffer            Pointer to the buffer where remaining FIFO data will be stored.
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param AD5940_FIFO_count          Pointer to a variable to retrieve the remaining FIFO count.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_stop(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count
);

/**
 * @brief Handles FIFO interrupts during Chronoamperometry (CA) operation.
 * 
 * @param MCU_FIFO_buffer            Pointer to the buffer to store FIFO data.
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param AD5940_FIFO_new_thresh     Sets a new FIFO threshold. If set to 0, the AD5940 shuts down.
 * @param AD5940_FIFO_count          Pointer to retrieve the current FIFO count.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_interrupt(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    const uint16_t AD5940_FIFO_new_thresh,
    uint16_t *const AD5940_FIFO_count
);

#ifdef __cplusplus
}
#endif
