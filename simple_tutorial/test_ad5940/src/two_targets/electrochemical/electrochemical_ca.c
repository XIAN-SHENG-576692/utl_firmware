#include "electrochemical_ca.h"

#include <stdlib.h>
#include <math.h>

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CA_get_data_length(
    const ELECTROCHEMICAL_CA_PARAMETERS *const parameters,
    uint16_t *const length
)
{
    if(parameters->t_interval == 0) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    *length = floor(parameters->t_run / parameters->t_interval) + 1;
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CA_get_times(
    const ELECTROCHEMICAL_CA_PARAMETERS *const parameters,
    const uint16_t times_max_length,
    uint32_t *const times,
    uint16_t *const times_length
)
{
    ELECTROCHEMICAL_ERROR error;
    error = ELECTROCHEMICAL_CA_get_data_length(
        parameters,
        times_length
    );
    if(error) return error;
    if(times_max_length < *times_length) return ELECTROCHEMICAL_ERROR_OUT_OF_BUFFER;

    for(size_t i=0; i<(*times_length); i++)
    {
        times[i] = i * parameters->t_interval;
    }

    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CA_get_voltages(
    const ELECTROCHEMICAL_CA_PARAMETERS *const parameters,
    const uint16_t voltages_max_length,
    int16_t *const voltages,
    uint16_t *const voltages_length
)
{
    ELECTROCHEMICAL_ERROR error;
    error = ELECTROCHEMICAL_CA_get_data_length(
        parameters,
        voltages_length
    );
    if(error) return error;
    if(voltages_max_length < *voltages_length) return ELECTROCHEMICAL_ERROR_OUT_OF_BUFFER;

    for(size_t i=0; i<(*voltages_length); i++)
    {
        voltages[i] = parameters->e_dc;
    }

    return ELECTROCHEMICAL_ERROR_SUCCESS;
}
