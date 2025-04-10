#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

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
float AD5940_EXTERNAL_COMPONENTS_get_fRcal(void);

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
uint32_t AD5940_EXTERNAL_COMPONENTS_get_HSRTIA(void);

#ifdef __cplusplus
}
#endif
