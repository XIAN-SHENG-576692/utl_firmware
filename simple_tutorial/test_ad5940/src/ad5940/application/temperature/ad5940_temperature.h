#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_temperature_utility.h"

/**
 * @brief Configuration structure for starting the temperature measurement on the AD5940.
 */
typedef struct
{
    uint32_t FIFO_thresh;                           /**< FIFO threshold value. Interrupt is triggered when this threshold is reached. */
    uint16_t ADC_sample_interval;                   /**< ADC sampling interval in microseconds. */

    float LFOSC_frequency;                          /**< Low-frequency oscillator frequency, used for internal timing. 
                                                        Obtainable via @ref AD5940_LFOSCMeasure in library/ad5940.h.*/

    const AD5940_UTILITY_ClockConfig *clock;        /**< Clock configuration. Obtainable via 
                                                        @ref AD5940_UTILITY_set_active_power 
                                                        in utility/ad5940_utility_power.h. */

    const AGPIOCfg_Type *agpio_cfg;                 /**< GPIO configuration for temperature measurement. */

    const AD5940_TEMPERATURE_ANALOG_CONFIG *analog; /**< Analog configuration structure. 
                                                        Refer to @ref AD5940_TEMPERATURE_ANALOG_CONFIG. */

    uint32_t TEMPSENS;                              /**< Temperature sensor configuration. Refer to page 57 of the datasheet:
                                                        ```
                                                        For improved accuracy, configure the temperature sensor in chop
                                                        mode via TEMPCON0, Bits[3:1]. If chopping is selected, the user
                                                        must ensure an even number of ADC conversions take place on the
                                                        temperature sensor channel and that these results are averaged.
                                                        ```
                                                        See TEMPCON0 configuration details on page 61 of the datasheet. */
}
AD5940_TEMPERATURE_START_CONFIG;

/**
 * @brief Starts the temperature measurement process on the AD5940.
 *
 * This function initializes the AD5940 for temperature measurement by configuring GPIOs, 
 * ADC settings, FIFO thresholds, and interrupts. It uses the specified configuration 
 * structure to define measurement parameters such as sampling frequency and FIFO settings.
 *
 * @param config A pointer to an `AD5940_TEMPERATURE_START_CONFIG` structure that 
 *               contains all the required configuration parameters.
 * 
 * @return AD5940Err Returns an error code of type `AD5940Err`. A value of 0 indicates 
 *                   success, while any other value represents an error during initialization.
 */
AD5940Err AD5940_TEMPERATURE_start(
    const AD5940_TEMPERATURE_START_CONFIG *const config
);

/**
 * @brief Stops the temperature measurement process and retrieves any remaining FIFO data.
 *
 * This function halts the AD5940's temperature measurement operation, retrieves the 
 * remaining FIFO data, and transfers it to the MCU buffer.
 *
 * @param MCU_FIFO_buffer            Pointer to the MCU buffer to store the remaining AD5940 FIFO data.
 * @param MCU_FIFO_buffer_max_length Maximum allowable length of the MCU buffer.
 * @param AD5940_FIFO_count          Pointer to a variable where the remaining FIFO count will be stored.
 * 
 * @return AD5940Err Returns an error code of type `AD5940Err`. A value of 0 indicates success, 
 *                   while any other value represents an error during the stop operation.
 */
AD5940Err AD5940_TEMPERATURE_stop(
    uint32_t* MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count
);

/**
 * @brief Handles FIFO interrupts during temperature measurement on the AD5940.
 *
 * This function processes FIFO interrupts by reading data from the AD5940 FIFO buffer, 
 * transferring it to the MCU buffer, and optionally updating the FIFO threshold. 
 * If the new FIFO threshold is set to 0, the AD5940 will shut down temperature measurements.
 *
 * @param MCU_FIFO_buffer            Pointer to the MCU buffer to store FIFO data from the AD5940.
 * @param MCU_FIFO_buffer_max_length Maximum allowable length of the MCU buffer.
 * @param AD5940_FIFO_new_thresh     New FIFO threshold to set. If 0, the AD5940 will stop measurements.
 * @param AD5940_FIFO_count          Pointer to a variable where the current FIFO count will be stored.
 *                                   This count can be retrieved even if an error is raised.
 * 
 * @return AD5940Err Returns an error code of type `AD5940Err`. A value of 0 indicates success, 
 *                   while any other value represents an error encountered during interrupt handling.
 */
AD5940Err AD5940_TEMPERATURE_interrupt(
    uint32_t* MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    const uint16_t AD5940_FIFO_new_thresh,
    uint16_t* AD5940_FIFO_count
);

#ifdef __cplusplus
}
#endif
