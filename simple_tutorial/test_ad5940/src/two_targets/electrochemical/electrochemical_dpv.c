#include "electrochemical_dpv.h"

#include <stdlib.h>
#include <math.h>

static ELECTROCHEMICAL_ERROR _get_e_step_real(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    int16_t *const e_step_real
)
{
    if(parameters->e_step == 0) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    *e_step_real = (parameters->e_end > parameters->e_begin) 
        ? parameters->e_step 
        : -parameters->e_step;
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

static ELECTROCHEMICAL_ERROR _get_e_pulse_real(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    int16_t *const e_pulse
)
{
    if(parameters->e_pulse == 0) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    if(!ELECTROCHEMICAL_DPV_IS_INVERSION_OPTION(parameters->inversion_option)) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    switch (parameters->inversion_option)
    {
    case ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_NONE:
        *e_pulse = (parameters->e_end > parameters->e_begin) ? parameters->e_pulse : -(parameters->e_pulse);
        break;

    case ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_BOTH:
        *e_pulse = (parameters->e_end > parameters->e_begin) ? (-parameters->e_pulse) : parameters->e_pulse;
        break;
    
    case ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_CATHODIC:
        *e_pulse = parameters->e_pulse;
        break;
    
    case ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_ANODIC:
        *e_pulse = (-parameters->e_pulse);
        break;
    }
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_voltage_max(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    int16_t *const voltage
)
{
    ELECTROCHEMICAL_ERROR error;
    int16_t e_pulse_real;
    error = _get_e_pulse_real(
        parameters,
        &e_pulse_real
    );
    if(error) return error;
    if(parameters->e_begin > parameters->e_end) 
    {
        *voltage = MAX(parameters->e_begin, (parameters->e_begin + e_pulse_real));
    }
    else
    {
        *voltage = MAX(parameters->e_end, (parameters->e_end + e_pulse_real));
    }
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_voltage_min(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    int16_t *const voltage
)
{
    ELECTROCHEMICAL_ERROR error;
    int16_t e_pulse_real;
    error = _get_e_pulse_real(
        parameters,
        &e_pulse_real
    );
    if(error) return error;
    if(parameters->e_begin < parameters->e_end) 
    {
        *voltage = MIN(parameters->e_begin, (parameters->e_begin + e_pulse_real));
    }
    else
    {
        *voltage = MIN(parameters->e_end, (parameters->e_end + e_pulse_real));
    }
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_t_interval(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    uint16_t *const time
)
{
    if(parameters->scan_rate == 0) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    uint16_t t_interval = 1E3F * ((float) (parameters->e_step)) / ((float) parameters->scan_rate);
    if(parameters->t_pulse >= t_interval) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    *time = t_interval;
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

static ELECTROCHEMICAL_ERROR _get_step_number(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    uint16_t *const step_number
)
{
    if(parameters->e_step == 0) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;
    *step_number = (floor(abs((parameters->e_end - parameters->e_begin) / parameters->e_step)) + 1);
    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_data_length(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    const ELECTROCHEMICAL_DPV_ARRAY_OPTION array_option,
    uint16_t *const length
)
{
    if(!ELECTROCHEMICAL_DPV_IS_ARRAY_OPTION(array_option)) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;

    ELECTROCHEMICAL_ERROR error;
    uint16_t step_number;

    error = _get_step_number(
        parameters,
        &step_number
    );
    if(error) return error;

    if(array_option == ELECTROCHEMICAL_DPV_ARRAY_OPTION_BOTH)
    {
        *length = step_number * 2;
    }
    else
    {
        *length = step_number;
    }

    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_times(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    const ELECTROCHEMICAL_DPV_ARRAY_OPTION array_option,
    const uint16_t times_max_length,
    uint32_t *const times,
    uint16_t *const times_length
)
{
    if(!ELECTROCHEMICAL_DPV_IS_ARRAY_OPTION(array_option)) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;

    ELECTROCHEMICAL_ERROR error;

    error = ELECTROCHEMICAL_DPV_get_data_length(
        parameters,
        array_option,
        times_length
    );
    if(error) return error;
    if(times_max_length < *times_length) return ELECTROCHEMICAL_ERROR_OUT_OF_BUFFER;
    
    uint16_t t_interval;
    uint16_t step_number;

    error = ELECTROCHEMICAL_DPV_get_t_interval(
        parameters,
        &t_interval
    );
    if(error) return error;

    error = _get_step_number(
        parameters,
        &step_number
    );
    if(error) return error;


    for(size_t i=0; i<step_number; i++)
    {
        switch (array_option)
        {
        case ELECTROCHEMICAL_DPV_ARRAY_OPTION_STEP:
            times[i] = t_interval * i;
            break;
    
        case ELECTROCHEMICAL_DPV_ARRAY_OPTION_PULSE:
            times[i] = (t_interval * i + 1) - t_interval;
            break;
        
        case ELECTROCHEMICAL_DPV_ARRAY_OPTION_BOTH:
            times[2 * i] = t_interval * i;
            times[2 * i + 1] = (t_interval * i + 1) - t_interval;
            break;
        }
    }

    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_get_voltages(
    const ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    const ELECTROCHEMICAL_DPV_ARRAY_OPTION array_option,
    const uint16_t voltages_max_length,
    int16_t *const voltages,
    uint16_t *const voltages_length
)
{
    if(!ELECTROCHEMICAL_DPV_IS_ARRAY_OPTION(array_option)) return ELECTROCHEMICAL_ERROR_WRONG_PARAMETERS;

    ELECTROCHEMICAL_ERROR error;

    error = ELECTROCHEMICAL_DPV_get_data_length(
        parameters,
        array_option,
        voltages_length
    );
    if(error) return error;
    if(voltages_max_length < *voltages_length) return ELECTROCHEMICAL_ERROR_OUT_OF_BUFFER;

    int16_t e_step_real;
    int16_t e_pulse_real;
    uint16_t step_number;

    error = _get_e_step_real(
        parameters,
        &e_step_real
    );
    if(error) return error;

    error = _get_e_pulse_real(
        parameters,
        &e_pulse_real
    );
    if(error) return error;

    error = _get_step_number(
        parameters,
        &step_number
    );
    if(error) return error;

    switch (array_option)
    {
    case ELECTROCHEMICAL_DPV_ARRAY_OPTION_STEP:
	    voltages[0] = parameters->e_begin;
        break;

    case ELECTROCHEMICAL_DPV_ARRAY_OPTION_PULSE:
        voltages[0] = parameters->e_begin + e_pulse_real;
        break;
    
    case ELECTROCHEMICAL_DPV_ARRAY_OPTION_BOTH:
        voltages[0] = parameters->e_begin;
        voltages[1] = parameters->e_begin + e_pulse_real;
        break;
    }

    for(size_t i=1; i<step_number; i++)
    {
        switch (array_option)
        {
        case ELECTROCHEMICAL_DPV_ARRAY_OPTION_STEP:
        case ELECTROCHEMICAL_DPV_ARRAY_OPTION_PULSE:
            voltages[i] = voltages[i - 1] + e_step_real;
            break;
        
        case ELECTROCHEMICAL_DPV_ARRAY_OPTION_BOTH:
            voltages[2 * i] = voltages[2 * (i - 1)] + e_step_real;
            voltages[2 * i + 1] = voltages[2 * (i - 1) + 1] + e_step_real;
            break;
        }
    }

    return ELECTROCHEMICAL_ERROR_SUCCESS;
}

ELECTROCHEMICAL_ERROR ELECTROCHEMICAL_DPV_map_currents_from_measurement_to_result(
    const int32_t *const measurement_currents, 
    const uint16_t measurement_currents_length,
    const uint16_t result_currents_max_length,
    int32_t *const result_currents, 
    uint16_t *const result_currents_length
)
{
    *result_currents_length = measurement_currents_length / 2;
    if(result_currents_max_length < (*result_currents_length)) return ELECTROCHEMICAL_ERROR_OUT_OF_BUFFER;

    for(size_t i=0; i<(*result_currents_length); i++)
    {
        result_currents[i] = measurement_currents[2 * i + 1] - measurement_currents[2 * i];
    }

    return ELECTROCHEMICAL_ERROR_SUCCESS;
}
