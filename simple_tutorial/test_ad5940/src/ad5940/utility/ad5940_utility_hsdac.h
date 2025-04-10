#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"

/**
 * @param EXCTBUFGAIN   @see page 43 of the datasheet, select from EXCTBUFGAIN_2, EXCTBUFGAIN_0P25 @ref EXCITBUFGAIN_Const
*/
AD5940Err AD5940_UTILITY_EXCITBUFGAIN_to_inampgnmde(
    const uint32_t EXCTBUFGAIN,
    double *const inampgnmde
);

/**
 * @param HSDACGAIN     @see page 43 of the datasheet, select from HSDACGAIN_1, HSDACGAIN_0P2 @ref HSDACGAIN_Const
*/
AD5940Err AD5940_UTILITY_HSDACGAIN_to_attenen(
    const uint32_t HSDACGAIN,
    double *const attenen
);

typedef enum {
    AD5940_UTILITY_HSDACDAT_HANDLER_CEIL,
    AD5940_UTILITY_HSDACDAT_HANDLER_FLOOR,
    AD5940_UTILITY_HSDACDAT_HANDLER_ROUND,
} 
AD5940_UTILITY_HSDACDAT_HANDLER;

/**
 * @param voltage           V
 * @param hsdac_dat_bits    @see page 43 of the datasheet, get HSDACDAT
*/
AD5940Err AD5940_UTILITY_voltage_to_hsdac_dat_bits(
    const double voltage,
    const AD5940_UTILITY_HSDACDAT_HANDLER handler,
    const uint32_t EXCTBUFGAIN, 
    const uint32_t HSDACGAIN, 
    uint32_t *const hsdac_dat_bits
);

/**
 * @param hsdac_dat_bits    @see page 43 of the datasheet, get HSDACDAT
 * @param voltage           V
*/
AD5940Err AD5940_UTILITY_hsdac_dat_bits_to_voltage(
    const uint32_t hsdac_dat_bits,
    const uint32_t EXCTBUFGAIN, 
    const uint32_t HSDACGAIN, 
    double *const voltage
);

#ifdef __cplusplus
}
#endif
