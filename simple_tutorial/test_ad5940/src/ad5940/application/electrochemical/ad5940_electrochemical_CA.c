#include "ad5940_electrochemical_CA.h"

#include "ad5940.h"
#include "ad5940_utility.h"

#include "ad5940_electrochemical_utility.h"

static AD5940Err _check_parameters(
    const AD5940_ELECTROCHEMICAL_CA_PARAMETERS *const parameters
)
{
    if(parameters->t_interval == 0) return AD5940ERR_PARA;
    if(parameters->t_run == 0) return AD5940ERR_PARA;
    if(parameters->t_run < parameters->t_interval) return AD5940ERR_PARA;
    return AD5940ERR_OK;
}

AD5940Err AD5940_ELECTROCHEMICAL_CA_get_times(
    const AD5940_ELECTROCHEMICAL_CA_PARAMETERS *const parameters,
    uint32_t *const times,
    uint16_t *const times_length,
    const uint16_t times_max_length
)
{
    AD5940Err error = _check_parameters(parameters);
    if(error != AD5940ERR_OK) return error;

    *times_length = parameters->t_run / parameters->t_interval;
    if(times_max_length < *times_length) return AD5940ERR_PARA;
    for(size_t i=0; i<*times_length; i++)
    {
        times[i] = parameters->t_interval * i;
    }
    return AD5940ERR_OK;
}

AD5940Err AD5940_ELECTROCHEMICAL_CA_get_voltages(
    const AD5940_ELECTROCHEMICAL_CA_PARAMETERS *const parameters,
    int32_t *const voltages,
    uint16_t *const voltages_length,
    const uint16_t voltages_max_length
)
{
    AD5940Err error = _check_parameters(parameters);
    if(error != AD5940ERR_OK) return error;

    *voltages_length = parameters->t_run / parameters->t_interval;
    if(voltages_max_length < *voltages_length) return AD5940ERR_PARA;
    for(size_t i=0; i<*voltages_length; i++)
    {
        voltages[i] = parameters->E_dc;
    }
    return AD5940ERR_OK;
}

static AD5940Err _write_sequence_commands(
    const ADCFilterCfg_Type *const adc_filter,
    const DFTCfg_Type *const dft,
    const AD5940_UTILITY_ClockConfig *const clock,
    uint32_t DataType
)
{
    AD5940Err error = AD5940ERR_OK;
    
    uint32_t sequence_address = 0x00;

    error = AD5940_ELECTROCHEMICAL_UTILITY_write_sequence_commands_config(
        &sequence_address,
        adc_filter,
        dft,
        clock,
        DataType
    );
    if(error != AD5940ERR_OK) return AD5940ERR_PARA;

    return AD5940ERR_OK;
}

static AD5940Err _start_wakeup_timer_sequence(
    const AD5940_ELECTROCHEMICAL_CA_PARAMETERS *parameters,
    const uint32_t FifoSrc, 
    const float LFOSC_frequency
)
{
    /* Configure FIFO and Sequencer for normal Amperometric Measurement */
    AD5940_FIFOThrshSet(parameters->t_run / parameters->t_interval);
    AD5940_FIFOCtrlS(FifoSrc, bTRUE);

    AD5940_SEQCtrlS(bTRUE);

    SEQInfo_Type *ADC_seq_info;
    AD5940_ELECTROCHEMICAL_UTILITY_get_ADC_seq_info(
        &ADC_seq_info
    );

    /* Configure Wakeup Timer*/
    WUPTCfg_Type wupt_cfg;
    wupt_cfg.WuptEn = bTRUE;
    wupt_cfg.WuptEndSeq = WUPTENDSEQ_A;
    wupt_cfg.WuptOrder[0] = ADC_seq_info->SeqId;
    wupt_cfg.SeqxSleepTime[ADC_seq_info->SeqId] = 1; /* The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock. */
    wupt_cfg.SeqxWakeupTime[ADC_seq_info->SeqId] = (uint32_t)(LFOSC_frequency * parameters->t_interval * 1E-3F) - 1;
    AD5940_WUPTCfg(&wupt_cfg);

    return AD5940ERR_OK;
}

/**
 * This function is based on the example in the AppCHRONOAMPInit() function found in
 * ad5940-examples/examples/AD5940_ChronoAmperometric/ChronoAmperometric.c.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_start_with_LPTIA(
    const AD5940_ELECTROCHEMICAL_CA_LPTIA_CONFIG *const config
)
{
    AD5940Err error = AD5940ERR_OK;

    error = _check_parameters(config->parameters);
    if(error != AD5940ERR_OK) return error;

    /* Wakeup AFE by read register, read 10 times at most */
    if(AD5940_WakeUp(10) > 10) return AD5940ERR_WAKEUP;  /* Wakeup Failed */

    /**
     * Before the application begins, INT are used for configuring parameters.
     * Therefore, they should not be used during the configuration process itself.
     */
    AD5940_UTILITY_clear_GPIO_and_INT_flag();

    error = AD5940_ELECTROCHEMICAL_UTILITY_AFE_LPDAC_LPTIA_config(
        config->parameters->E_dc,
        config->parameters->E_dc,
        config->utility_AFERefCfg_Type
    );
    if(error != AD5940ERR_OK) return error;

    error = _write_sequence_commands(
        &(config->utility_DSPCfg_Type->ADCFilterCfg),
        &(config->utility_DSPCfg_Type->DftCfg),
        config->clock,
        config->DataType
    );
    if(error != AD5940ERR_OK) return error;

    error = AD5940_ELECTROCHEMICAL_UTILITY_LPDAC_LPTIA_ADC_config(
        config->utility_LPPACfg_Type,
        config->utility_LPTIACfg_Type,
        config->utility_DSPCfg_Type
    );
    if(error != AD5940ERR_OK) return error;

    // Ensure it is cleared as ad5940.c relies on the INTC flag as well.
    AD5940_INTCClrFlag(AFEINTSRC_ALLINT);

    AGPIOCfg_Type agpio_cfg;
    memcpy(&agpio_cfg, config->agpio_cfg, sizeof(AGPIOCfg_Type));
    AD5940_UTILITY_set_INTCCfg_by_AGPIOCfg_Type(&agpio_cfg, AFEINTSRC_DATAFIFOTHRESH);
    AD5940_AGPIOCfg(&agpio_cfg);

    error = _start_wakeup_timer_sequence(
        config->parameters,
        config->FifoSrc,
        config->LFOSC_frequency
    );
    if(error != AD5940ERR_OK) return error;

    return error;
}

/**
 * This function is based on the example in the AppCHRONOAMPInit() function and 
 * AppCHRONOAMPCtrl() function found in
 * ad5940-examples/examples/AD5940_ChronoAmperometric/ChronoAmperometric.c.
 * 
 * However, the official example code uses LPDAC, LPTIA, and LPADC for electrochemical measurements, 
 * which does not support switching between different working electrodes. 
 * Therefore, we need to modify it to use HSTIA instead.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_start(
    const AD5940_ELECTROCHEMICAL_CA_CONFIG *const config
)
{
    AD5940Err error = AD5940ERR_OK;

    error = _check_parameters(config->parameters);
    if(error != AD5940ERR_OK) return error;

    /* Wakeup AFE by read register, read 10 times at most */
    if(AD5940_WakeUp(10) > 10) return AD5940ERR_WAKEUP;  /* Wakeup Failed */

    /**
     * Before the application begins, INT are used for configuring parameters.
     * Therefore, they should not be used during the configuration process itself.
     */
    AD5940_UTILITY_clear_GPIO_and_INT_flag();

    error = AD5940_ELECTROCHEMICAL_UTILITY_AFE_LPDAC_HSTIA_config(
        config->parameters->E_dc,
        config->parameters->E_dc,
        config->utility_AFERefCfg_Type
    );
    if(error != AD5940ERR_OK) return error;

    error = _write_sequence_commands(
        &(config->utility_DSPCfg_Type->ADCFilterCfg),
        &(config->utility_DSPCfg_Type->DftCfg),
        config->clock,
        config->DataType
    );
    if(error != AD5940ERR_OK) return error;

    error = AD5940_ELECTROCHEMICAL_UTILITY_LPDAC_HSTIA_ADC_config(
        config->working_electrode,
        config->utility_LPPACfg_Type,
        config->utility_HSTIACfg_Type,
        config->utility_DSPCfg_Type
    );
    if(error != AD5940ERR_OK) return error;

    // Ensure it is cleared as ad5940.c relies on the INTC flag as well.
    AD5940_INTCClrFlag(AFEINTSRC_ALLINT);

    AGPIOCfg_Type agpio_cfg;
    memcpy(&agpio_cfg, config->agpio_cfg, sizeof(AGPIOCfg_Type));
    AD5940_UTILITY_set_INTCCfg_by_AGPIOCfg_Type(&agpio_cfg, AFEINTSRC_DATAFIFOTHRESH);
    AD5940_AGPIOCfg(&agpio_cfg);

    error = _start_wakeup_timer_sequence(
        config->parameters,
        config->FifoSrc,
        config->LFOSC_frequency
    );
    if(error != AD5940ERR_OK) return error;

    return error;
}

AD5940Err AD5940_ELECTROCHEMICAL_CA_stop(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count
)
{
    return AD5940_ELECTROCHEMICAL_CA_interrupt(
        MCU_FIFO_buffer, 
        MCU_FIFO_buffer_max_length,
        0,
        AD5940_FIFO_count
    );
}

/**
 * This function is based on the example in the AppCHRONOAMPISR() function found in
 * ad5940-examples/examples/AD5940_ChronoAmperometric/ChronoAmperometric.c.
 */
AD5940Err AD5940_ELECTROCHEMICAL_CA_interrupt(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    const uint16_t AD5940_FIFO_new_thresh,
    uint16_t *const AD5940_FIFO_count
)
{
    /* Wakeup AFE by read register, read 10 times at most */
    if(AD5940_WakeUp(10) > 10) return AD5940ERR_WAKEUP;  /* Wakeup Failed */

    AD5940_SleepKeyCtrlS(SLPKEY_LOCK);  /* We need time to read data from FIFO, so, do not let AD5940 goes to hibernate automatically */

    *AD5940_FIFO_count = AD5940_FIFOGetCnt();
    if(*AD5940_FIFO_count > MCU_FIFO_buffer_max_length) return AD5940ERR_BUFF;
    AD5940_FIFORd(MCU_FIFO_buffer, *AD5940_FIFO_count);

    AD5940_SleepKeyCtrlS(SLPKEY_UNLOCK); /* Unlock so sequencer can put AD5940 to sleep */

    if(AD5940_FIFO_new_thresh > 0)
    {
        AD5940_FIFOThrshSet(AD5940_FIFO_new_thresh);
        AD5940_INTCClrFlag(AFEINTSRC_DATAFIFOTHRESH);
        AD5940_EnterSleepS();
    }
    else
    {
        AD5940_INTCClrFlag(AFEINTSRC_DATAFIFOTHRESH);
        AD5940_UTILITY_shutdown();
    }
    
    return 0;
}
