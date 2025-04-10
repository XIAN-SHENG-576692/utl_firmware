#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"

typedef enum
{
    AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_SE0,
    AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_DE0,
    AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_AIN0,
    AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_AIN1,
    AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_AIN2,
    AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_AIN3,
}
AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE;

/**
 * @brief Configures the switch matrix for the specified working electrode.
 * 
 * This function maps the selected working electrode to the corresponding configuration 
 * in the switch matrix.
 * 
 * @note
 * - Refer to pages 30 to 52 and 74 to 76 of the datasheet for details.
 * - Figure 36 (page 76) illustrates the switch matrix diagram.
 * - The switch matrix only supports HSTIA. If LPTIA is used, only `SE0` is supported 
 *   as the working electrode.
 * 
 * @param type              Pointer to a `SWMatrixCfg_Type` structure to store the configuration.
 * @param working_electrode The working electrode to be selected. Choose from 
 *                          `AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE`.
 * 
 * @return AD5940Err       Returns an error code of type `AD5940Err`. A value of 0 indicates success, 
 *                         while other values indicate a specific error.
 */
AD5940Err AD5940_ELECTROCHEMICAL_UTILITY_get_switch_matrix_by_working_electrode(
    SWMatrixCfg_Type *const type,
    const AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE working_electrode
);

#ifdef __cplusplus
}
#endif
