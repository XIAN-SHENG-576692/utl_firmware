#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940_electrochemical_ca_struct.h"

/**
 * @brief Configuration for Chronoamperometry (CA) using LPTIA.
 */
typedef struct 
{
    const AD5940_ELECTROCHEMICAL_CA_PARAMETERS *parameters;     /**< Pointer to the CA parameters. */
    uint16_t fifo_thresh; /**< Sets a new FIFO threshold. */

    float LFOSC_frequency;  /**< Low-frequency oscillator frequency, used for internal timing. 
                                 Obtainable via @ref AD5940_LFOSCMeasure in library/ad5940.h.*/
                                                        
    const AD5940_ClockConfig *clock;    /**< Pointer to clock configuration.
                                                     Obtainable via 
                                                     @ref AD5940_set_active_power 
                                                     in utility/ad5940_utility_power.h. */

    const AGPIOCfg_Type *agpio_cfg;     /**< Pointer to GPIO configuration. 
                                             - Refer to datasheet pages 112 and 122.
                                             - Configure GPIO for interrupts based on PCB design. */

    const AD5940_ELECTROCHEMICAL_AFERefCfg_Type *afe_ref_cfg;    /**< Pointer to AFE reference configuration. */
    const AD5940_ELECTROCHEMICAL_LPDACfg_Type *lpdac_cfg;        /**< Pointer to LPPA configuration. */
    const AD5940_ELECTROCHEMICAL_LPTIACfg_Type *lptia_cfg;      /**< Pointer to LPTIA configuration. */
    const AD5940_ELECTROCHEMICAL_DSPCfg_Type *dsp_cfg;      /**< Pointer to DSP configuration. */

    uint32_t DataType; /**< Data type configuration. @ref DATATYPE_Const. */
    uint32_t FifoSrc;  /**< FIFO source configuration. @ref FIFOSRC_Const*/
} 
AD5940_ELECTROCHEMICAL_CA_LPDAC_LPTIA_CONFIG;

/**
 * @brief Starts the Chronoamperometry (CA) operation using LPTIA configuration.
 * 
 * @param config Pointer to the LPTIA CA configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_start_with_LPDAC_LPTIA(
    const AD5940_ELECTROCHEMICAL_CA_LPDAC_LPTIA_CONFIG *const config
);

/**
 * @brief Configuration for Chronoamperometry (CA) with a specified working electrode.
 */
typedef struct 
{
    const AD5940_ELECTROCHEMICAL_CA_PARAMETERS *parameters;     /**< Pointer to the CA parameters. */
    uint16_t fifo_thresh; /**< Sets a new FIFO threshold. */

    const AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING *electrode_routing;

    float LFOSC_frequency;  /**< Low-frequency oscillator frequency, used for internal timing. 
                                 Obtainable via @ref AD5940_LFOSCMeasure in library/ad5940.h.*/
                                                        
    const AD5940_ClockConfig *clock;    /**< Pointer to clock configuration.
                                                     Obtainable via 
                                                     @ref AD5940_set_active_power 
                                                     in utility/ad5940_utility_power.h. */

    const AGPIOCfg_Type *agpio_cfg;     /**< Pointer to GPIO configuration. 
                                             - Refer to datasheet pages 112 and 122.
                                             - Configure GPIO for interrupts based on PCB design. */

    const AD5940_ELECTROCHEMICAL_AFERefCfg_Type *afe_ref_cfg;    /**< Pointer to AFE reference configuration. */
    const AD5940_ELECTROCHEMICAL_LPDACfg_Type *lpdac_cfg;        /**< Pointer to LPPA configuration. */
    const AD5940_ELECTROCHEMICAL_HSTIACfg_Type *hstia_cfg;      /**< Pointer to HSTIA configuration. */
    const AD5940_ELECTROCHEMICAL_DSPCfg_Type *dsp_cfg;      /**< Pointer to DSP configuration. */

    uint32_t DataType; /**< Data type configuration. @ref DATATYPE_Const. */
    uint32_t FifoSrc;  /**< FIFO source configuration. @ref FIFOSRC_Const*/
} 
AD5940_ELECTROCHEMICAL_CA_LPDAC_HSTIA_CONFIG;

/**
 * @brief Starts the Chronoamperometry (CA) operation.
 * 
 * @param config Pointer to the CA configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_start_with_LPDAC_HSTIA(
    const AD5940_ELECTROCHEMICAL_CA_LPDAC_HSTIA_CONFIG *const config
);

/**
 * @brief Stops the Chronoamperometry (CA) operation and shuts down the AD5940.
 * 
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param MCU_FIFO_buffer            Pointer to the buffer where remaining FIFO data will be stored.
 * @param MCU_FIFO_count             Pointer to a variable to retrieve the remaining FIFO count.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_stop(
    const uint16_t MCU_FIFO_buffer_max_length,
    uint32_t *const MCU_FIFO_buffer, 
    uint16_t *const MCU_FIFO_count
);

/**
 * @brief Handles FIFO interrupts during Chronoamperometry (CA) operation.
 * 
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param AD5940_FIFO_new_thresh     Sets a new FIFO threshold. If set to 0, the AD5940 shuts down.
 * @param MCU_FIFO_buffer            Pointer to the buffer to store FIFO data.
 * @param MCU_FIFO_count             Pointer to retrieve the current FIFO count.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_interrupt(
    const uint16_t MCU_FIFO_buffer_max_length,
    const uint16_t AD5940_FIFO_new_thresh,
    uint32_t *const MCU_FIFO_buffer, 
    uint16_t *const MCU_FIFO_count
);

#ifdef __cplusplus
}
#endif
