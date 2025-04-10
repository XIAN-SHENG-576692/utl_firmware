#include "electrochemical_cv.h"

#include <stdlib.h>
#include <math.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_voltage_max(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    int16_t *const voltage
)
{
    if(parameters->e_begin > parameters->e_vertex1 && parameters->e_begin > parameters->e_vertex2) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    if(parameters->e_begin < parameters->e_vertex1 && parameters->e_begin < parameters->e_vertex2) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    *voltage = MAX(parameters->e_vertex1, parameters->e_vertex2);
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_voltage_min(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    int16_t *const voltage
)
{
    if(parameters->e_begin > parameters->e_vertex1 && parameters->e_begin > parameters->e_vertex2) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    if(parameters->e_begin < parameters->e_vertex1 && parameters->e_begin < parameters->e_vertex2) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    *voltage = MIN(parameters->e_vertex1, parameters->e_vertex2);
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_t_interval(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    uint16_t *const time
)
{
    if(parameters->scan_rate == 0) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    *time = 1E3F * ((float) (parameters->e_step)) / ((float) parameters->scan_rate);
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

static ELECTROCHEMICAL_ERROR _get_e_step_real(
    const int16_t e_begin,
    const int16_t e_end,
    const int16_t e_step,
    int16_t *const e_step_real
)
{
    if(e_step == 0) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    *e_step_real = (e_end > e_begin) 
        ? e_step 
        : -e_step;
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

static ELECTROCHEMICAL_ERROR _get_step_number_ramp(
    const int16_t e_begin,
    const int16_t e_end,
    const int16_t e_step,
    uint16_t *const step_number_ramp
)
{
    if(e_step == 0) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    *step_number_ramp = floor(abs(e_end - e_begin) / e_step);
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

static ELECTROCHEMICAL_ERROR _get_step_number(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    uint16_t *const step_number
)
{
    ELECTROCHEMICAL_ERROR error;
    uint16_t step_number_ramp_buffer;
    uint16_t step_number_buffer = 0;

    error = _get_step_number_ramp(
        parameters->e_begin,
        parameters->e_vertex1,
        parameters->e_step,
        &step_number_ramp_buffer
    );
    if(error) return error;
    step_number_buffer += step_number_ramp_buffer;

    error = _get_step_number_ramp(
        parameters->e_vertex1,
        parameters->e_vertex2,
        parameters->e_step,
        &step_number_ramp_buffer
    );
    if(error) return error;
    step_number_buffer += step_number_ramp_buffer;

    error = _get_step_number_ramp(
        parameters->e_vertex2,
        parameters->e_begin,
        parameters->e_step,
        &step_number_ramp_buffer
    );
    if(error) return error;
    step_number_buffer += step_number_ramp_buffer;

    *step_number = step_number_buffer;
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_data_length(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    uint16_t *const length
)
{
    ELECTROCHEMICAL_ERROR error;
    uint16_t step_number;

    error = _get_step_number(
        parameters,
        &step_number
    );
    if(error) return error;

    *length = step_number * parameters->number_of_scans + 1;
    
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_times(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint16_t times_max_length,
    uint32_t *const times,
    uint16_t *const times_length
) 
{
    ELECTROCHEMICAL_ERROR error;

    error = ELECTROCHEMICAL_CV_get_data_length(
        parameters,
        times_length
    );
    if(error) return error;
    if(times_max_length < *times_length) return ELECTROCHEMICAL_ERROR_OUT_OF_BUFFER;

    uint16_t t_interval;
    uint16_t step_number;

    error = ELECTROCHEMICAL_CV_get_t_interval(
        parameters,
        &t_interval
    );
    if(error) return error;

    error = _get_step_number(
        parameters,
        &step_number
    );
    if(error) return error;

    for(size_t i=0; i<parameters->number_of_scans; i++)
    {
        for(size_t j=0; j<step_number; j++)
        {
            times[((i * step_number) + j)] = t_interval * ((i * step_number) + j);
        }
    }
    times[(*times_length) - 1] = t_interval * (parameters->number_of_scans * step_number);
    
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_CV_get_voltages(
    const ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint16_t voltages_max_length,
    int16_t *const voltages,
    uint16_t *const voltages_length
) 
{
    ELECTROCHEMICAL_ERROR error;

    error = ELECTROCHEMICAL_CV_get_data_length(
        parameters,
        voltages_length
    );
    if(error) return error;
    if(voltages_max_length < *voltages_length) return ELECTROCHEMICAL_ERROR_OUT_OF_BUFFER;

    int16_t e_step_real_begin_to_vertex1;
    int16_t e_step_real_vertex1_to_vertex2;
    int16_t e_step_real_vertex2_to_begin;
    uint16_t step_number;
    uint16_t step_number_ramp_begin_to_vertex1;
    uint16_t step_number_ramp_vertex1_to_vertex2;
    uint16_t step_number_ramp_vertex2_to_begin;

    error = _get_e_step_real(
        parameters->e_begin,
        parameters->e_vertex1,
        parameters->e_step,
        &e_step_real_begin_to_vertex1
    );
    if(error) return error;

    error = _get_e_step_real(
        parameters->e_vertex1,
        parameters->e_vertex2,
        parameters->e_step,
        &e_step_real_vertex1_to_vertex2
    );
    if(error) return error;

    error = _get_e_step_real(
        parameters->e_vertex2,
        parameters->e_begin,
        parameters->e_step,
        &e_step_real_vertex2_to_begin
    );
    if(error) return error;

    error = _get_step_number(
        parameters,
        &step_number
    );
    if(error) return error;

    error = _get_step_number_ramp(
        parameters->e_begin,
        parameters->e_vertex1,
        parameters->e_step,
        &step_number_ramp_begin_to_vertex1
    );
    if(error) return error;

    error = _get_step_number_ramp(
        parameters->e_vertex1,
        parameters->e_vertex2,
        parameters->e_step,
        &step_number_ramp_vertex1_to_vertex2
    );
    if(error) return error;

    error = _get_step_number_ramp(
        parameters->e_vertex2,
        parameters->e_begin,
        parameters->e_step,
        &step_number_ramp_vertex2_to_begin
    );
    if(error) return error;
    
    voltages[0] = parameters->e_begin;
    uint16_t index = 0;
    for(size_t i=0; i<step_number_ramp_begin_to_vertex1; i++)
    {
        voltages[index + 1] = voltages[index] + e_step_real_begin_to_vertex1;
        index++;
    }
    for(size_t i=0; i<step_number_ramp_vertex1_to_vertex2; i++)
    {
        voltages[index + 1] = voltages[index] + e_step_real_vertex1_to_vertex2;
        index++;
    }
    for(size_t i=0; i<step_number_ramp_vertex2_to_begin; i++)
    {
        voltages[index + 1] = voltages[index] + e_step_real_vertex2_to_begin;
        index++;
    }

    for(size_t i=0; i<parameters->number_of_scans; i++)
    {
        for(size_t j=0; j<step_number; j++)
        {
            voltages[((i * step_number) + j)] = voltages[j];
        }
    }
    voltages[(*voltages_length) - 1] = voltages[0];

    return ELECTROCHEMICAL_ERROR_SUCCESS;
}
