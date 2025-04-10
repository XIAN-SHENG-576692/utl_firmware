/**
 * @file ad5940_electrochemical_utility.h
 * @brief Provides common operations for AD5940 electrochemical applications.
 * 
 * This file includes utility functions and configurations related to AD5940's 
 * electrochemical operations. Key features include:
 * - Writing ADC sample sequence commands.
 * - Configuring SRAM to distribute sequence storage and FIFO management.
 * - Configuring ADC filter and DFT settings for electrochemical measurements.
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_utility.h"
#include "ad5940_electrochemical_utility.h"

/**
 * @brief Retrieves the sequence information for ADC sampling.
 * 
 * This function provides a pointer to the sequence information used for 
 * ADC sampling in electrochemical measurements.
 * 
 * @param ADC_seq_info Pointer to a `SEQInfo_Type` pointer where the ADC sequence 
 *                     information will be stored.
 */
void AD5940_ELECTROCHEMICAL_UTILITY_get_ADC_seq_info(
    SEQInfo_Type **ADC_seq_info
);

/**
 * @brief Writes the configuration commands for ADC sequence operations.
 * 
 * This function writes the necessary sequence commands to configure the ADC, 
 * DFT, and clock settings for electrochemical measurements. The configuration 
 * details are based on the provided parameters, and the sequence address is updated.
 * 
 * @param sequence_address Pointer to store the address of the written sequence.
 * @param adc_filter       Pointer to the ADC filter configuration structure. 
 *                         See `ADCFilterCfg_Type` for details.
 * @param dft              Pointer to the DFT configuration structure. 
 *                         See `DFTCfg_Type` for details.
 * @param clock            Clock configuration. Obtainable via 
 *                         @ref AD5940_UTILITY_set_active_power 
 *                         in utility/ad5940_utility_power.h.
 * @param DataType         The data type configuration for ADC outputs. 
 *                         Refer to @ref DATATYPE_Const for options.
 * 
 * @return AD5940Err       Error code indicating success or failure of the operation:
 *                         - `AD5940Err_OK`: Operation was successful.
 *                         - Other values indicate specific errors.
 */
AD5940Err AD5940_ELECTROCHEMICAL_UTILITY_write_sequence_commands_config(
    uint32_t *const sequence_address,
    const ADCFilterCfg_Type *const adc_filter,
    const DFTCfg_Type *const dft,
    const AD5940_UTILITY_ClockConfig *const clock,
    const uint32_t DataType
);

#ifdef __cplusplus
}
#endif
