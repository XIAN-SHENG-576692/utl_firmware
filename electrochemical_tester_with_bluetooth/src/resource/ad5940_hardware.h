#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"

/**
 * Retrieves the value of the RCAL resistor.
 * 
 * @note 
 * Refer to page 46 of the datasheet for more details. 
 * This function must be implemented to match your specific board design.
 * 
 * @return The resistance value (in ohms) of the resistor connected between 
 *         the RCAL0 pin and the RCAL1 pin.
 */
#define AD5940_EXTERNAL_fRcal 10000.0    // Referring to our circuit, We connect a 10K ohm resistor between the RCAL0 pin and the RCAL1 pin.

/**
 * Retrieves the resistance value of the HSRTIA (High-Speed RTIA).
 * 
 * @note 
 * This function must be implemented to match your specific board design. 
 * The HSRTIA resistance is used only when the HSTIA is powered on and 
 * `HstiaRtiaSel` is set to `HSTIARTIA_OPEN`.
 * 
 * @return The resistance value (in ohms) of the HSRTIA.
 */
#define AD5940_EXTERNAL_HSRTIA 0     // Our project didn't use it.

// Our circuit use GPIO7 as interrupt pin.
extern AGPIOCfg_Type AD5940_EXTERNAL_agpio_cfg;

#ifdef __cplusplus
}
#endif
