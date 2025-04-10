#include "ad5940_electrochemical_cv_function.h"

#include "ad5940_utils.h"
#include "ad5940_electrochemical_utils.h"

#include <stdlib.h>

#define DAC_0_SEQID SEQID_1
#define DAC_1_SEQID SEQID_2

#define E_STEP_REAL(e_begin, e_end, e_step) ((e_end > e_begin) ? e_step : -e_step)
static inline uint16_t STEP_NUMBER_RAMP(float e_begin, float e_end, float e_step) {
    float total = fabsf((e_end - e_begin) / e_step);
    float intpart;
    float frac = modff(total, &intpart);

    if (frac > 1e-7f) {  // If the fractional part is greater than epsilon, round up unconditionally
        return (uint16_t)(intpart + 1.0f);
    } else {
        return (uint16_t)(intpart);  // Otherwise, treat it as an integer without rounding up
    }
}
#define STEP_NUMBER(parameters) (\
    STEP_NUMBER_RAMP(\
        parameters->e_begin,\
        parameters->e_vertex1,\
        parameters->e_step\
    ) + \
    STEP_NUMBER_RAMP(\
        parameters->e_vertex1,\
        parameters->e_vertex2,\
        parameters->e_step\
    ) + \
    STEP_NUMBER_RAMP(\
        parameters->e_vertex2,\
        parameters->e_begin,\
        parameters->e_step\
    )\
)

static float _get_voltage_at_index(
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint16_t index,
    float e_step_real_b1,
    float e_step_real_12,
    float e_step_real_2b,
    uint16_t step_number_b1,
    uint16_t step_number_b12,
    uint16_t step_number_b12b
) 
{
    uint16_t position = index % step_number_b12b;
    if (position < step_number_b1) {
        return parameters->e_begin + ((position) * e_step_real_b1);
    } else if (position < step_number_b12) {
        return parameters->e_vertex1 + ((position - step_number_b1) * e_step_real_12);
    } else {
        return parameters->e_vertex2 + ((position - step_number_b12) * e_step_real_2b);
    }
}

/* Geneate sequence(s) to update DAC step by step */
/* Note: this function doesn't need sequencer generator */

/**
* @brief Update DAC sequence in SRAM in real time.  
* @details This function generates sequences to update DAC code step by step. It's also called in interrupt
*          function when half commands in SRAM has been completed. We don't use sequence generator to save memory.
*          Check more details from documentation of this example. @ref Ramp_Test_Example
* @return return error code
* 
* */
static AD5940Err _write_DAC_sequence_commands(
	const uint32_t start_address,
    uint32_t *const sequence_length,
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters
)
{
    AD5940Err error;

    error = AD5940_ELECTROCHEMICAL_CV_PARAMETERS_check(parameters);
    if(error != AD5940ERR_OK) return error;

	#define SEQLEN_ONESTEP 3L  /* How many sequence commands are needed to update LPDAC. */

	uint32_t SeqCmdBuff[SEQLEN_ONESTEP];

	BoolFlag next_is_dac0 = bFALSE;

	uint32_t current_address = start_address;
    uint32_t lpdac_dat_bit;

    float e_step_real_b1;
    float e_step_real_12;
    float e_step_real_2b;
    e_step_real_b1 = E_STEP_REAL(
        parameters->e_begin,
        parameters->e_vertex1,
        parameters->e_step
    );
    e_step_real_12 = E_STEP_REAL(
        parameters->e_vertex1,
        parameters->e_vertex2,
        parameters->e_step
    );
    e_step_real_2b = E_STEP_REAL(
        parameters->e_vertex2,
        parameters->e_begin,
        parameters->e_step
    );
    uint16_t step_number_b1;
    uint16_t step_number_b12;
    uint16_t step_number_b12b;
    step_number_b1 = STEP_NUMBER_RAMP(
        parameters->e_begin,
        parameters->e_vertex1,
        parameters->e_step
    );
    step_number_b12 = step_number_b1 + STEP_NUMBER_RAMP(
        parameters->e_vertex1,
        parameters->e_vertex2,
        parameters->e_step
    );
    step_number_b12b = step_number_b12 + STEP_NUMBER_RAMP(
        parameters->e_vertex2,
        parameters->e_begin,
        parameters->e_step
    );

    float e_current;

    size_t length = step_number_b12b;
    // if(length % 2 == 1) length *= 2;

    for(size_t i=0; i<length; i++)
    {
        e_current = _get_voltage_at_index(
            parameters,
            i,
            e_step_real_b1,
            e_step_real_12,
            e_step_real_2b,
            step_number_b1,
            step_number_b12,
            step_number_b12b
        );
        error = AD5940_ELECTROCHEMICAL_calculate_lpdac_dat_bits_by_potential(
            e_current,
            &lpdac_dat_bit
        );
        if(error != AD5940ERR_OK) return error;
        SeqCmdBuff[0] = SEQ_WR(REG_AFE_LPDACDAT0, lpdac_dat_bit);
        SeqCmdBuff[1] = SEQ_WAIT(10); /* !!!NOTE LPDAC need 10 clocks to update data. Before send AFE to sleep state, wait 10 extra clocks */
        AD5940_get_change_sequence_info_command(
            (next_is_dac0) ? DAC_0_SEQID : DAC_1_SEQID,
            (i == (length - 1)) 
                ? start_address     // Turn back to the first point.
                : (current_address + SEQLEN_ONESTEP),
            SEQLEN_ONESTEP,
            SeqCmdBuff + 2
        );
        AD5940_SEQCmdWrite(current_address, SeqCmdBuff, SEQLEN_ONESTEP);
        next_is_dac0 = (next_is_dac0 == bTRUE) ? bFALSE : bTRUE;
        current_address += SEQLEN_ONESTEP;
    }
    *sequence_length = current_address - start_address;

    AD5940_write_change_sequence_info_command(
        DAC_0_SEQID,
        start_address,
        SEQLEN_ONESTEP
    );

    AD5940_write_change_sequence_info_command(
        DAC_1_SEQID,
        start_address + SEQLEN_ONESTEP,
        SEQLEN_ONESTEP
    );

	return AD5940ERR_OK;
}

static AD5940Err _write_sequence_commands(
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const ADCFilterCfg_Type *const adc_filter,
    const DFTCfg_Type *const dft,
    const AD5940_ClockConfig *const clock,
    uint32_t DataType
)
{
    AD5940Err error = AD5940ERR_OK;

    uint32_t sequence_address = 0x00;
    uint32_t sequence_commands_length = 0;

    error = AD5940_ELECTROCHEMICAL_write_sequence_commands_config(
        adc_filter,
        dft,
        clock,
        DataType,
        &sequence_address
    );
    if(error != AD5940ERR_OK) return AD5940ERR_PARA;

    error = _write_DAC_sequence_commands(
        sequence_address,
        &sequence_commands_length,
        parameters
    );
    if(error != AD5940ERR_OK) return AD5940ERR_PARA;
    sequence_address += sequence_commands_length;

    return AD5940ERR_OK;
}

#define FIFO_THRESH(parameters) (STEP_NUMBER(parameters))

static AD5940Err _start_wakeup_timer_sequence(
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint32_t FifoSrc, 
    const float LFOSC_frequency
)
{
    float t_interval;
    AD5940_ELECTROCHEMICAL_CV_get_t_interval(
        parameters,
        &t_interval
    );

    /* Configure FIFO and Sequencer for normal Amperometric Measurement */
    AD5940_FIFOThrshSet(FIFO_THRESH(parameters));
    AD5940_FIFOCtrlS(FifoSrc, bTRUE);

    AD5940_SEQCtrlS(bTRUE);

    #define SAMPLE_DELAY 0.025
    
    SEQInfo_Type *ADC_seq_info;
    AD5940_ELECTROCHEMICAL_UTILITY_get_ADC_seq_info(
        &ADC_seq_info
    );

    /* Configure Wakeup Timer*/
	WUPTCfg_Type wupt_cfg;
	wupt_cfg.WuptEn = bTRUE;
	wupt_cfg.WuptEndSeq = WUPTENDSEQ_D;
	wupt_cfg.WuptOrder[0] = DAC_0_SEQID;
	wupt_cfg.WuptOrder[1] = ADC_seq_info->SeqId;
	wupt_cfg.WuptOrder[2] = DAC_1_SEQID;
	wupt_cfg.WuptOrder[3] = ADC_seq_info->SeqId;
	wupt_cfg.SeqxSleepTime[ADC_seq_info->SeqId] = 1;    // The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock.
	wupt_cfg.SeqxWakeupTime[ADC_seq_info->SeqId] = (uint32_t)(LFOSC_frequency * SAMPLE_DELAY) - 1;
	wupt_cfg.SeqxSleepTime[DAC_0_SEQID] = 1;            // The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock.
	wupt_cfg.SeqxWakeupTime[DAC_0_SEQID] = (uint32_t)(LFOSC_frequency * (t_interval - SAMPLE_DELAY)) - 1;
	wupt_cfg.SeqxSleepTime[DAC_1_SEQID] = 1;            // The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock.
	wupt_cfg.SeqxWakeupTime[DAC_1_SEQID] = (uint32_t)(LFOSC_frequency * (t_interval - SAMPLE_DELAY)) - 1;
    AD5940_WUPTCfg(&wupt_cfg);

    return AD5940ERR_OK;
}

AD5940Err AD5940_ELECTROCHEMICAL_CV_start_with_LPDAC_LPTIA(
    const AD5940_ELECTROCHEMICAL_CV_LPDAC_LPTIA_CONFIG *const config
)
{
    AD5940Err error = AD5940ERR_OK;

    error = AD5940_ELECTROCHEMICAL_CV_PARAMETERS_check(config->parameters);
    if(error != AD5940ERR_OK) return error;

    /* Wakeup AFE by read register, read 10 times at most */
    if(AD5940_WakeUp(10) > 10) return AD5940ERR_WAKEUP;  /* Wakeup Failed */

    /**
     * Before the application begins, INT are used for configuring parameters.
     * Therefore, they should not be used during the configuration process itself.
     */
    AD5940_clear_GPIO_and_INT_flag();

    error = AD5940_ELECTROCHEMICAL_config_afe_lpdac_lptia(
        config->afe_ref_cfg,
        config->parameters->e_begin
    );
    if(error != AD5940ERR_OK) return error;

    error = _write_sequence_commands(
        config->parameters,
        &(config->dsp_cfg->ADCFilterCfg),
        &(config->dsp_cfg->DftCfg),
        config->clock,
        config->DataType
    );
    if(error != AD5940ERR_OK) return error;

    error = AD5940_ELECTROCHEMICAL_config_lpdac_lptia_adc(
        config->lpdac_cfg,
        config->lptia_cfg,
        config->dsp_cfg
    );
    if(error != AD5940ERR_OK) return error;

    // Ensure it is cleared as ad5940.c relies on the INTC flag as well.
    AD5940_INTCClrFlag(AFEINTSRC_ALLINT);

    AGPIOCfg_Type agpio_cfg;
    memcpy(&agpio_cfg, config->agpio_cfg, sizeof(AGPIOCfg_Type));
    AD5940_set_INTCCfg_by_AGPIOCfg_Type(&agpio_cfg, AFEINTSRC_DATAFIFOTHRESH);
    AD5940_AGPIOCfg(&agpio_cfg);

    error = _start_wakeup_timer_sequence(
        config->parameters,
        config->FifoSrc,
        config->LFOSC_frequency
    );
    if(error != AD5940ERR_OK) return error;

    return error;
}

AD5940Err AD5940_ELECTROCHEMICAL_CV_start_with_LPDAC_HSTIA(
    const AD5940_ELECTROCHEMICAL_CV_LPDAC_HSTIA_CONFIG *const config
)
{
    AD5940Err error = AD5940ERR_OK;

    error = AD5940_ELECTROCHEMICAL_CV_PARAMETERS_check(config->parameters);
    if(error != AD5940ERR_OK) return error;

    /* Wakeup AFE by read register, read 10 times at most */
    if(AD5940_WakeUp(10) > 10) return AD5940ERR_WAKEUP;  /* Wakeup Failed */

    /**
     * Before the application begins, INT are used for configuring parameters.
     * Therefore, they should not be used during the configuration process itself.
     */
    AD5940_clear_GPIO_and_INT_flag();

    error = AD5940_ELECTROCHEMICAL_config_afe_lpdac_hstia(
        config->afe_ref_cfg,
        config->parameters->e_begin
    );
    if(error != AD5940ERR_OK) return error;

    error = _write_sequence_commands(
        config->parameters,
        &(config->dsp_cfg->ADCFilterCfg),
        &(config->dsp_cfg->DftCfg),
        config->clock,
        config->DataType
    );
    if(error != AD5940ERR_OK) return error;

    error = AD5940_ELECTROCHEMICAL_config_lpdac_hstia_adc(
        config->lpdac_cfg,
        config->hstia_cfg,
        config->dsp_cfg,
        config->electrode_routing
    );
    if(error != AD5940ERR_OK) return error;

    // Ensure it is cleared as ad5940.c relies on the INTC flag as well.
    AD5940_INTCClrFlag(AFEINTSRC_ALLINT);

    AGPIOCfg_Type agpio_cfg;
    memcpy(&agpio_cfg, config->agpio_cfg, sizeof(AGPIOCfg_Type));
    AD5940_set_INTCCfg_by_AGPIOCfg_Type(&agpio_cfg, AFEINTSRC_DATAFIFOTHRESH);
    AD5940_AGPIOCfg(&agpio_cfg);

    error = _start_wakeup_timer_sequence(
        config->parameters,
        config->FifoSrc,
        config->LFOSC_frequency
    );
    if(error != AD5940ERR_OK) return error;

    return error;
}

AD5940Err AD5940_ELECTROCHEMICAL_CV_stop(
    const uint16_t MCU_FIFO_buffer_max_length,
    uint32_t *const MCU_FIFO_buffer, 
    uint16_t *const MCU_FIFO_count
)
{
    return AD5940_ELECTROCHEMICAL_CV_interrupt(
        MCU_FIFO_buffer_max_length,
        MCU_FIFO_buffer, 
        MCU_FIFO_count,
        bFALSE
    );
}

AD5940Err AD5940_ELECTROCHEMICAL_CV_interrupt(
    const uint16_t MCU_FIFO_buffer_max_length,
    uint32_t *const MCU_FIFO_buffer, 
    uint16_t *const MCU_FIFO_count,
    const BoolFlag isContinue
)
{
    /* Wakeup AFE by read register, read 10 times at most */
    if(AD5940_WakeUp(10) > 10) return AD5940ERR_WAKEUP;  /* Wakeup Failed */

    AD5940_SleepKeyCtrlS(SLPKEY_LOCK);  /* We need time to read data from FIFO, so, do not let AD5940 goes to hibernate automatically */

    *MCU_FIFO_count = AD5940_FIFOGetCnt();
    if(*MCU_FIFO_count > MCU_FIFO_buffer_max_length) return AD5940ERR_BUFF;
    AD5940_FIFORd(MCU_FIFO_buffer, *MCU_FIFO_count);

    AD5940_INTCClrFlag(AFEINTSRC_DATAFIFOTHRESH);

    if (isContinue)
    {
        AD5940_reset_fifocon();
    }
    else 
    {
        AD5940_SleepKeyCtrlS(SLPKEY_UNLOCK); /* Unlock so sequencer can put AD5940 to sleep */
        AD5940_shutdown_afe_lploop_hsloop_dsp();
    }
    
    return AD5940ERR_OK;
}
