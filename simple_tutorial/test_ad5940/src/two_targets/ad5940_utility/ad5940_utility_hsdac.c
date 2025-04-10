#include "ad5940_utility_hsdac.h"

AD5940Err AD5940_UTILITY_EXCITBUFGAIN_to_inampgnmde(
    const uint32_t EXCTBUFGAIN,
    double *const inampgnmde
)
{
    switch (EXCTBUFGAIN)
    {
    case EXCITBUFGAIN_2:
        *inampgnmde = 2.0;
        break;
    case EXCITBUFGAIN_0P25:
        *inampgnmde = 0.25;
        break;
    default:
        return AD5940ERR_PARA;
    }
    return AD5940ERR_OK;
}

AD5940Err AD5940_UTILITY_HSDACGAIN_to_attenen(
    const uint32_t HSDACGAIN,
    double *const attenen
)
{
    switch (HSDACGAIN)
    {
    case HSDACGAIN_1:
        *attenen = 1.0;
        break;
    case HSDACGAIN_0P2:
        *attenen = 0.2;
        break;
    default:
        return AD5940ERR_PARA;
    }
    return AD5940ERR_OK;
}

AD5940Err AD5940_UTILITY_voltage_to_hsdac_dat_bits(
    const double voltage,
    const AD5940_UTILITY_HSDACDAT_HANDLER handler,
    const uint32_t EXCTBUFGAIN, 
    const uint32_t HSDACGAIN, 
    uint32_t *const hsdac_dat_bits
)
{
    AD5940Err error;
    double inampgnmde;
    double attenen;

    error = AD5940_UTILITY_EXCITBUFGAIN_to_inampgnmde(EXCTBUFGAIN, &inampgnmde);
    if(error) return error;
    error = AD5940_UTILITY_HSDACGAIN_to_attenen(HSDACGAIN, &attenen);
    if(error) return error;

    uint32_t bits;
    switch (handler)
    {
    case AD5940_UTILITY_HSDACDAT_HANDLER_CEIL:
        bits = (uint32_t) ceilf(-(voltage * 1e3 * 2048 / inampgnmde / attenen / 404.4) + 2048);
        break;
    case AD5940_UTILITY_HSDACDAT_HANDLER_FLOOR:
        bits = (uint32_t) floorf(-(voltage * 1e3 * 2048 / inampgnmde / attenen / 404.4) + 2048);
        break;
    case AD5940_UTILITY_HSDACDAT_HANDLER_ROUND:
        bits = (uint32_t) roundf(-(voltage * 1e3 * 2048 / inampgnmde / attenen / 404.4) + 2048);
        break;
    default:
        return AD5940ERR_PARA;
    }

    if(bits < 0x200) return AD5940ERR_PARA;
    if(bits > 0xE00) return AD5940ERR_PARA;
    *hsdac_dat_bits = bits;
    return AD5940ERR_OK;
}

AD5940Err AD5940_UTILITY_hsdac_dat_bits_to_voltage(
    const uint32_t hsdac_dat_bits,
    const uint32_t EXCTBUFGAIN, 
    const uint32_t HSDACGAIN, 
    double *const voltage
)
{
    AD5940Err error;
    double inampgnmde;
    double attenen;

    error = AD5940_UTILITY_EXCITBUFGAIN_to_inampgnmde(EXCTBUFGAIN, &inampgnmde);
    if(error) return error;
    error = AD5940_UTILITY_HSDACGAIN_to_attenen(HSDACGAIN, &attenen);
    if(error) return error;

    if(hsdac_dat_bits < 0x200) return AD5940ERR_PARA;
    if(hsdac_dat_bits > 0xE00) return AD5940ERR_PARA;
    *voltage = -(hsdac_dat_bits - 2048) * inampgnmde * attenen * 404.4 / 2048 / 1e3;
    return AD5940ERR_OK;
}
