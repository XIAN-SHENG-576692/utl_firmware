#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "electrochemical.h"

#include <stdint.h>

/**
 * @brief Parameters for the AD5940 Electrochemical Chronoamperometry (CA) operation.
 */
typedef struct 
{
    int16_t e_dc;           /**< DC potential applied during the experiment in millivolts (mV). */
    uint16_t t_interval;    /**< Time interval between measurements in milliseconds (ms). */
    uint32_t t_run;         /**< Total runtime for the experiment in milliseconds (ms). */
}
ELECTROCHEMICAL_CA_PARAMETERS;

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CA_get_data_length(
    const ELECTROCHEMICAL_CA_PARAMETERS *const parameters,
    uint16_t *const length
);

/**
 * @brief Calculates the time points for Chronoamperometry (CA) measurements.
 * 
 * This function generates an array of time points based on the provided CA parameters.
 * 
 * @param parameters                Pointer to the CA parameters structure.
 * @param times_max_length          Maximum allowable length of the `times` array.
 * @param times                     Pointer to an array to store calculated time points.
 * @param times_length              Pointer to a variable to store the number of generated time points.
 *
 * @return ELECTROCHEMICAL_ERROR    Error code indicating success (0) or failure.
 */
ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CA_get_times(
    const ELECTROCHEMICAL_CA_PARAMETERS *const parameters,
    const uint16_t times_max_length,
    uint32_t *const times,
    uint16_t *const times_length
);

/**
 * @brief Calculates the voltage values for Chronoamperometry (CA) measurements.
 * 
 * This function generates an array of voltage values based on the provided CA parameters.
 * 
 * @param parameters                Pointer to the CA parameters structure.
 * @param voltages_max_length       Maximum allowable length of the `voltages` array.
 * @param voltages                  Pointer to an array to store calculated voltages.
 * @param voltages_length           Pointer to a variable to store the number of generated voltages.
 * 
 * @return ELECTROCHEMICAL_ERROR    Error code indicating success (0) or failure.
 */
ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CA_get_voltages(
    const ELECTROCHEMICAL_CA_PARAMETERS *const parameters,
    const uint16_t voltages_max_length,
    int16_t *const voltages,
    uint16_t *const voltages_length
);

#ifdef __cplusplus
}
#endif
