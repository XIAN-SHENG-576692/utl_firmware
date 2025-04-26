#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940_electrochemical_cv_struct.h"

#include "ad5940_electrochemical_utils_loop.h"
#include "ad5940_electrochemical_utils_run.h"

/**
 * @brief Configuration structure for Cyclic Voltammetry (CV).
 * 
 * This structure allows the user to configure the parameters and signal paths 
 * required for performing electrochemical CV measurements.
 * 
 * Only one signal path should be specified among the following options:
 * - lpdac_to_lptia
 * - lpdac_to_hstia
 * - hsdac_mmr_to_hstia
 * 
 * The selected path determines the loop used to perform the electrochemical operation.
 */
typedef struct 
{
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *parameters;                    /**< DPV parameter settings */
    const AD5940_ELECTROCHEMICAL_RUN_CONFIG *run;                               /**< Execution and timing configuration */
    const AD5940_ELECTROCHEMICAL_LPDAC_TO_LPTIA_CONFIG *lpdac_to_lptia;         /**< Configuration for LPDAC to LPTIA path */
    const AD5940_ELECTROCHEMICAL_LPDAC_TO_HSTIA_CONFIG *lpdac_to_hstia;         /**< Configuration for LPDAC to HSTIA path */
    const AD5940_ELECTROCHEMICAL_HSDAC_MMR_TO_HSTIA_CONFIG *hsdac_mmr_to_hstia; /**< Configuration for HSDAC via MMR to HSTIA path */
} 
AD5940_ELECTROCHEMICAL_CV_CONFIG;

/**
 * @brief Starts the Cyclic Voltammetry (CV) operation.
 * 
 * @param config Pointer to the CV configuration structure.
 * 
 * @return AD5940Err Error code indicating success (0) or failure.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CV_start(
    const AD5940_ELECTROCHEMICAL_CV_CONFIG *const config
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
