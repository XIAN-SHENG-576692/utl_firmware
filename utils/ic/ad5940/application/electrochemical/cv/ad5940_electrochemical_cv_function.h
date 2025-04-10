#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940_electrochemical_cv_struct.h"

/**
 * @brief Configuration for Cyclic Voltammetry (CV) using LPTIA.
 */
typedef struct 
{
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *parameters;     /**< Pointer to the CV parameters. */

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
AD5940_ELECTROCHEMICAL_CV_LPDAC_LPTIA_CONFIG;

/**
 * @brief Starts the Cyclic Voltammetry (CV) operation using LPTIA configuration.
 * 
 * @param config Pointer to the LPTIA CV configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_start_with_LPDAC_LPTIA(
    const AD5940_ELECTROCHEMICAL_CV_LPDAC_LPTIA_CONFIG *const config
);

/**
 * @brief Configuration for Cyclic Voltammetry (CV) with a specified working electrode.
 */
typedef struct 
{
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *parameters;     /**< Pointer to the CV parameters. */

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
AD5940_ELECTROCHEMICAL_CV_LPDAC_HSTIA_CONFIG;

/**
 * @brief Starts the Cyclic Voltammetry (CV) operation.
 * 
 * @param config Pointer to the CV configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_start_with_LPDAC_HSTIA(
    const AD5940_ELECTROCHEMICAL_CV_LPDAC_HSTIA_CONFIG *const config
);

/**
 * @brief Stops the Cyclic Voltammetry (CV) operation and shuts down the AD5940.
 * 
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param MCU_FIFO_buffer            Pointer to the buffer where remaining FIFO data will be stored.
 * @param MCU_FIFO_count             Pointer to a variable to retrieve the remaining FIFO count.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_stop(
    const uint16_t MCU_FIFO_buffer_max_length,
    uint32_t *const MCU_FIFO_buffer, 
    uint16_t *const MCU_FIFO_count
);

/**
 * @brief Handles FIFO interrupts during Cyclic Voltammetry (CV) operation.
 * 
 * @param MCU_FIFO_buffer_max_length Maximum length of the MCU FIFO buffer.
 * @param MCU_FIFO_buffer            Pointer to the buffer to store FIFO data.
 * @param MCU_FIFO_count             Pointer to retrieve the current FIFO count.
 * @param isContinue                 Flag to indicate whether to continue the CV operation.
 * 
 * @return AD5940Err                 Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_interrupt(
    const uint16_t MCU_FIFO_buffer_max_length,
    uint32_t *const MCU_FIFO_buffer, 
    uint16_t *const MCU_FIFO_count,
    const BoolFlag isContinue
);

#ifdef __cplusplus
}
#endif
