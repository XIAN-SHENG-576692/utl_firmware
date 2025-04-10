#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "electrochemical.h"

#include <stdint.h>

/**
 * @brief Parameters for the AD5940 Electrochemical Cyclic Voltammetry (CV) operation.
 */
typedef struct 
{
    int16_t e_begin;            /**< Starting potential of the scan, in millivolts (mV). */
    int16_t e_vertex1;          /**< First vertex potential of the scan, in millivolts (mV). */
    int16_t e_vertex2;          /**< Second vertex potential of the scan, in millivolts (mV). */
    uint16_t e_step;            /**< Step potential between measurements, in millivolts (mV). */
    uint16_t scan_rate;         /**< Rate of potential change during the scan, in millivolts per second (mV/s). */
    uint16_t number_of_scans;   /**< The number of scans for which time values need to be generated. */
} 
ELECTROCHEMICAL_CV_PARAMETERS;

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_voltage_max(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    int16_t *const voltage
);

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_voltage_min(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    int16_t *const voltage
);

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_t_interval(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    uint16_t *const time
);

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_data_length(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    uint16_t *const length
);

/**
 * @brief Generates time values for Cyclic Voltammetry (CV) measurements.
 * 
 * This function calculates and populates an array of time values based on the provided
 * Cyclic Voltammetry (CV) parameters and the specified number of scans.
 * 
 * @param parameters                Pointer to the structure containing CV parameters.
 * @param times_max_length          The maximum capacity of the `times` array.
 * @param times                     Pointer to an array where the calculated time values will be stored.
 * @param times_length              Pointer to a variable to store the actual number of generated time values.
 * 
 * @return ELECTROCHEMICAL_ERROR    Error code indicating success (0) or failure.
 */
ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_times(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint16_t times_max_length,
    uint32_t *const times,
    uint16_t *const times_length
);

/**
 * @brief Generates voltage values for Cyclic Voltammetry (CV) measurements.
 * 
 * This function calculates and populates an array of voltage values based on the provided
 * Cyclic Voltammetry (CV) parameters and the specified number of scans.
 * 
 * @param parameters                Pointer to the structure containing CV parameters.
 * @param voltages_max_length       The maximum capacity of the `voltages` array.
 * @param voltages                  Pointer to an array where the calculated voltage values will be stored.
 * @param voltages_length           Pointer to a variable to store the actual number of generated voltage values.
 * 
 * @return ELECTROCHEMICAL_ERROR    Error code indicating success (0) or failure.
 */
ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_voltages(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint16_t voltages_max_length,
    int16_t *const voltages,
    uint16_t *const voltages_length
);

#ifdef __cplusplus
}
#endif
