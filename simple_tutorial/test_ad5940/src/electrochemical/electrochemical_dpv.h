#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "electrochemical.h"

#include <stdint.h>

/**
 * @brief Options for signal inversion in Differential Pulse Voltammetry (DPV) measurements.
 * 
 * This enumeration defines the possible inversion options that can be applied during DPV measurements.
 */
typedef enum {
    ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_NONE,      /**< No inversion is applied. */
    ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_BOTH,      /**< Invert both anodic and cathodic signals. */
    ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_CATHODIC,  /**< Invert only the cathodic signal. */
    ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_ANODIC,    /**< Invert only the anodic signal. */
} 
ELECTROCHEMICAL_DPV_INVERSION_OPTION;

/**
 * @brief Macro to validate if a given inversion option is valid.
 * 
 * This macro checks if the provided option is one of the defined inversion options.
 * 
 * @param option The inversion option to validate.
 * 
 * @return Non-zero if the option is valid, zero otherwise.
 */
#define ELECTROCHEMICAL_DPV_IS_INVERSION_OPTION(option) (\
    (option == ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_NONE) || \
    (option == ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_BOTH) || \
    (option == ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_CATHODIC) || \
    (option == ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_ANODIC) \
)

/**
 * @brief Parameters for the AD5940 Electrochemical Differential Pulse Voltammetry (DPV) operation.
 */
typedef struct 
{
    int16_t e_begin;    /**< Starting potential of the scan, in millivolts (mV). */
    int16_t e_end;      /**< Ending potential of the scan, in millivolts (mV). */
    uint16_t e_step;    /**< Step potential between measurements, in millivolts (mV). */
    uint16_t e_pulse;   /**< Pulse amplitude, in millivolts (mV). */
    uint16_t t_pulse;   /**< Pulse duration, in milliseconds (ms). */
    uint16_t scan_rate; /**< Rate of potential change during the scan, in millivolts per second (mV/s). */
    ELECTROCHEMICAL_DPV_INVERSION_OPTION inversion_option; /**< Option to invert the signal during measurement. */
} 
ELECTROCHEMICAL_DPV_PARAMETERS;

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_voltage_max(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    int16_t *const voltage
);

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_voltage_min(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    int16_t *const voltage
);

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_t_interval(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    uint16_t *const time
);

typedef enum {
    ELECTROCHEMICAL_DPV_ARRAY_OPTION_STEP,
    ELECTROCHEMICAL_DPV_ARRAY_OPTION_PULSE,
    ELECTROCHEMICAL_DPV_ARRAY_OPTION_BOTH,
} 
ELECTROCHEMICAL_DPV_ARRAY_OPTION;

#define ELECTROCHEMICAL_DPV_IS_ARRAY_OPTION(option) (\
    (option == ELECTROCHEMICAL_DPV_ARRAY_OPTION_STEP) || \
    (option == ELECTROCHEMICAL_DPV_ARRAY_OPTION_PULSE) || \
    (option == ELECTROCHEMICAL_DPV_ARRAY_OPTION_BOTH) \
)

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_data_length(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    const ELECTROCHEMICAL_DPV_ARRAY_OPTION array_option,
    uint16_t *const length
);

/**
 * @brief Generates time values for Differential Pulse Voltammetry (DPV) measurements.
 * 
 * This function calculates and populates an array of time values based on the provided
 * Differential Pulse Voltammetry (DPV) parameters and the specified number of scans.
 * 
 * @param parameters                Pointer to the structure containing DPV parameters.
 * @param times_max_length          The maximum capacity of the `times` array.
 * @param times                     Pointer to an array where the calculated time values will be stored.
 * @param times_length              Pointer to a variable to store the actual number of generated time values.
 * 
 * @return ELECTROCHEMICAL_ERROR    Error code indicating success (0) or failure.
 */
ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_times(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    const ELECTROCHEMICAL_DPV_ARRAY_OPTION array_option,
    const uint16_t times_max_length,
    uint32_t *const times,
    uint16_t *const times_length
);

/**
 * @brief Generates voltage values for Differential Pulse Voltammetry (DPV) measurements.
 * 
 * This function calculates and populates an array of voltage values based on the provided
 * Differential Pulse Voltammetry (DPV) parameters.
 * 
 * @param parameters                Pointer to the structure containing DPV parameters.
 * @param voltages_max_length       The maximum capacity of the `voltages` array.
 * @param voltages                  Pointer to an array where the calculated voltage values will be stored.
 * @param voltages_length           Pointer to a variable to store the actual number of generated voltage values.
 * 
 * @return ELECTROCHEMICAL_ERROR    Error code indicating success (0) or failure.
 */
ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_voltages(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    const ELECTROCHEMICAL_DPV_ARRAY_OPTION array_option,
    const uint16_t voltages_max_length,
    int16_t *const voltages,
    uint16_t *const voltages_length
);

/**
 * @brief Generates voltage values for Differential Pulse Voltammetry (DPV) measurements.
 * 
 * This function calculates and populates an array of voltage values based on the provided
 * Differential Pulse Voltammetry (DPV) parameters.
 * 
 * result_currents and measurement_currents allow be the same pointer.
 * 
 * @return ELECTROCHEMICAL_ERROR    Error code indicating success (0) or failure.
 */
ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_map_currents_from_measurement_to_result(
    const int32_t *const measurement_currents, 
    const uint16_t measurement_currents_length,
    const uint16_t result_currents_max_length,
    int32_t *const result_currents, 
    uint16_t *const result_currents_length
);

#ifdef __cplusplus
}
#endif
