#include "ad5940_utility_lpdac.h"

#include <stdlib.h>

/** 
 * Default LPDAC resolution(2.5V internal reference).
 * @see page 30 of the datasheet.
*/
#define DAC12BITVOLT_1LSB   (2.2 / 4095)            // mV
#define DAC6BITVOLT_1LSB    (DAC12BITVOLT_1LSB * 64)    // mV

AD5940Err AD5940_UTILITY_voltage_to_lpdac_dat_6_bits(
    const double voltage, 
    AD5940_UTILITY_LPDACDAT_HANDLER handler,
    uint16_t *const lpdac_dat_6_bits
)
{
    uint16_t code;
    switch (handler)
    {
    case AD5940_UTILITY_LPDACDAT_HANDLER_CEIL:
        code = (uint16_t) ceilf(voltage / DAC6BITVOLT_1LSB);
        break;
    case AD5940_UTILITY_LPDACDAT_HANDLER_FLOOR:
        code = (uint16_t) floorf(voltage / DAC6BITVOLT_1LSB);
        break;
    case AD5940_UTILITY_LPDACDAT_HANDLER_ROUND:
        code = (uint16_t) roundf(voltage / DAC6BITVOLT_1LSB);
        break;
    default:
        return AD5940ERR_PARA;
    }
    if(code > 0x3F) return AD5940ERR_PARA;
    *lpdac_dat_6_bits = code;
    return AD5940ERR_OK;
}

AD5940Err AD5940_UTILITY_voltage_to_lpdac_dat_12_bits(
    const double voltage, 
    AD5940_UTILITY_LPDACDAT_HANDLER handler,
    uint16_t *const lpdac_dat_12_bits
)
{
    uint16_t bits;
    switch (handler)
    {
    case AD5940_UTILITY_LPDACDAT_HANDLER_CEIL:
        bits = (uint16_t) ceilf(voltage / DAC12BITVOLT_1LSB);
        break;
    case AD5940_UTILITY_LPDACDAT_HANDLER_FLOOR:
        bits = (uint16_t) floorf(voltage / DAC12BITVOLT_1LSB);
        break;
    case AD5940_UTILITY_LPDACDAT_HANDLER_ROUND:
        bits = (uint16_t) roundf(voltage / DAC12BITVOLT_1LSB);
        break;
    default:
        return AD5940ERR_PARA;
    }
    if(bits > 0xFFF) return AD5940ERR_PARA;
    *lpdac_dat_12_bits = bits;
    return AD5940ERR_OK;
}

AD5940Err AD5940_UTILITY_lpdac_dat_6_bits_to_voltage(
    const uint16_t lpdac_dat_6_bits,
    double *const voltage
)
{
    if(lpdac_dat_6_bits > 0x3F) return AD5940ERR_PARA;
    *voltage = lpdac_dat_6_bits * DAC6BITVOLT_1LSB;
    return AD5940ERR_OK;
}

AD5940Err AD5940_UTILITY_lpdac_dat_12_bits_to_voltage(
    const uint16_t lpdac_dat_12_bits,
    double *const voltage
)
{
    if(lpdac_dat_12_bits > 0xFFF) return AD5940ERR_PARA;
    *voltage = lpdac_dat_12_bits * DAC12BITVOLT_1LSB;
    return AD5940ERR_OK;
}

AD5940Err AD5940_UTILITY_combine_lpdac_dat_bits(
    const uint16_t lpdac_dat_6_bits,
    const uint16_t lpdac_dat_12_bits,
    uint32_t *const lpdac_dat_bits
)
{
    *lpdac_dat_bits = (((uint32_t) (lpdac_dat_6_bits)) << 12) | ((uint32_t) (lpdac_dat_12_bits));
    return AD5940ERR_OK;
}

AD5940Err AD5940_UTILITY_get_LPDACDATBITS(
    const double V_out_max, 
    const double V_out, 
    uint16_t *const lpdac_dat_12_bit, 
    uint16_t *const lpdac_dat_6_bit
)
{
    if(V_out_max > 0) *lpdac_dat_6_bit = (uint16_t) (ceilf(V_out_max / DAC6BITVOLT_1LSB));
    else *lpdac_dat_6_bit = 0x00;

    if(*lpdac_dat_6_bit > 0x3F) return AD5940ERR_PARA;

    double V_cal;
    if(V_out_max > 0) V_cal = ((*lpdac_dat_6_bit) * DAC6BITVOLT_1LSB) - V_out_max;

    if(V_out_max > 0) *lpdac_dat_12_bit = (uint16_t) (roundf((V_out_max - V_out + V_cal) / DAC12BITVOLT_1LSB));
    else *lpdac_dat_12_bit = (uint16_t) (roundf((-V_out) / DAC12BITVOLT_1LSB));

    if(*lpdac_dat_12_bit > 0xFFF) return AD5940ERR_PARA;

    return AD5940ERR_OK;
}

AD5940Err AD5940_UTILITY_get_LPDACDATCFG(
    LPDACCfg_Type *const lpdac_cfg, 
    const double V_out_max, 
    const double V_out
)
{
    lpdac_cfg->LpDacVbiasMux = LPDACVBIAS_12BIT;
    lpdac_cfg->LpDacVzeroMux = LPDACVZERO_6BIT;
    return AD5940_UTILITY_get_LPDACDATBITS(
        V_out_max, V_out, &(lpdac_cfg->DacData12Bit), &(lpdac_cfg->DacData6Bit)
    );
}

AD5940Err AD5940_UTILITY_get_LPDACDATBIT(
    const double V_out_max, 
    const double V_out, 
    uint32_t *const lpdac_dat_bit
)
{
    AD5940Err error = AD5940ERR_OK;
    uint16_t lpdac_dat_12_bit, lpdac_dat_6_bit;
    error = AD5940_UTILITY_get_LPDACDATBITS(
        V_out_max, V_out, &lpdac_dat_12_bit, &lpdac_dat_6_bit
    );
    if(error != AD5940ERR_OK) return error;
    *lpdac_dat_bit = (((uint32_t) (lpdac_dat_6_bit)) << 12) | ((uint32_t) (lpdac_dat_12_bit));
    return AD5940ERR_OK;
}

AD5940Err AD5940_UTILITY_get_LPDAC_volt(
    const double V_out_max, 
    const double V_out, 
    uint16_t *const V_bias, 
    uint16_t *const V_zero
)
{
    AD5940Err error = AD5940ERR_OK;
    uint16_t lpdac_dat_12_bit, lpdac_dat_6_bit;
    error = AD5940_UTILITY_get_LPDACDATBITS(
        V_out_max, V_out, &lpdac_dat_12_bit, &lpdac_dat_6_bit
    );
    if(error != AD5940ERR_OK) return error;
    *V_bias = lpdac_dat_12_bit * DAC12BITVOLT_1LSB;
    *V_zero = lpdac_dat_6_bit * DAC6BITVOLT_1LSB;
    return AD5940ERR_OK;
}
