/**
 * Configures the voltage between V_bias and V_zero.
 * 
 * @note
 * Refer to page 30 of the datasheet for detailed information on setting the voltage
 * between V_bias and V_zero. The default MUX settings used in this file are:
 * - `LPDACVBIAS_12BIT` for V_bias (higher precision: 12 bits)
 * - `LPDACVZERO_6BIT` for V_zero (lower precision: 6 bits)
 * 
 * Since V_bias has higher precision than V_zero, the desired output voltage (V_out)
 * should be set as follows:
 * 1. Configure V_zero using the maximum voltage (V_max).
 * 2. Adjust V_bias to fine-tune the precision of V_out.
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"

typedef enum {
    AD5940_UTILITY_LPDACDAT_HANDLER_CEIL,
    AD5940_UTILITY_LPDACDAT_HANDLER_FLOOR,
    AD5940_UTILITY_LPDACDAT_HANDLER_ROUND,
} 
AD5940_UTILITY_LPDACDAT_HANDLER;

AD5940Err AD5940_UTILITY_voltage_to_lpdac_dat_6_bits(
    const double voltage, 
    AD5940_UTILITY_LPDACDAT_HANDLER handler,
    uint16_t *const lpdac_dat_6_bits
);

AD5940Err AD5940_UTILITY_voltage_to_lpdac_dat_12_bits(
    const double voltage, 
    AD5940_UTILITY_LPDACDAT_HANDLER handler,
    uint16_t *const lpdac_dat_12_bits
);

AD5940Err AD5940_UTILITY_lpdac_dat_6_bits_to_voltage(
    const uint16_t lpdac_dat_6_bits,
    double *const voltage
);

AD5940Err AD5940_UTILITY_lpdac_dat_12_bits_to_voltage(
    const uint16_t lpdac_dat_12_bits,
    double *const voltage
);

AD5940Err AD5940_UTILITY_combine_lpdac_dat_bits(
    const uint16_t lpdac_dat_6_bits,
    const uint16_t lpdac_dat_12_bits,
    uint32_t *const lpdac_dat_bits
);

/**
 * @see page 30 of the datasheet.
 * for LpDacVbiasMux to LPDACVBIAS_12BIT,
 *     LpDacVzeroMux to LPDACVZERO_6BIT,
 * @param V_out_max         V
 * @param V_out             V
 * @param lpdac_dat_12_bit  get 12-bit LPDACDAT
 * @param lpdac_dat_6_bit   get 6-bit LPDACDAT
*/
AD5940Err AD5940_UTILITY_get_LPDACDATBITS(
    const double V_out_max, 
    const double V_out, 
    uint16_t *const lpdac_dat_12_bit, 
    uint16_t *const lpdac_dat_6_bit
);

/**
 * @see page 30 of the datasheet.
 * @param lpdac_cfg set lpdac_cfg->LpDacVbiasMux to LPDACVBIAS_12BIT,
 *                  set lpdac_cfg->LpDacVzeroMux to LPDACVZERO_6BIT,
 *                  set lpdac_cfg->DacData12Bit
 *                  set lpdac_cfg->DacData6Bit
 * @param V_out_max mV
 * @param V_out     mV
*/
AD5940Err AD5940_UTILITY_get_LPDACDATCFG(
    LPDACCfg_Type *const lpdac_cfg, 
    const double V_out_max, 
    const double V_out
);

/**
 * @see page 30 of the datasheet.
 * @param V_out_max         V
 * @param V_out             V
 * @param lpdac_dat_12_bit  get LPDACDAT
*/
AD5940Err AD5940_UTILITY_get_LPDACDATBIT(
    const double V_out_max, 
    const double V_out, 
    uint32_t *const lpdac_dat_bit
);

/**
 * @param V_out_max V
 * @param V_out     V
 * @param V_bias0   V
 * @param V_zero0   V
*/
AD5940Err AD5940_UTILITY_get_LPDAC_volt(
    const double V_out_max, 
    const double V_out, 
    uint16_t *const V_bias, 
    uint16_t *const V_zero
);

#ifdef __cplusplus
}
#endif
