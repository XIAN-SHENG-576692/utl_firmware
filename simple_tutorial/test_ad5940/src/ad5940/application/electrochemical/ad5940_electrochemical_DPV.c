#include "ad5940_electrochemical_DPV.h"

#include "ad5940.h"
#include "ad5940_utility.h"

#include "ad5940_electrochemical_utility.h"

#include <stdlib.h>

#define DAC_REG_AFE_SEQINFO REG_AFE_SEQ1INFO
#define DAC_REG_AFE_SEQINFO_ADDR BITP_AFE_SEQ1INFO_ADDR
#define DAC_REG_AFE_SEQINFO_LEN BITP_AFE_SEQ1INFO_LEN

#define DAC_PULSE_REG_AFE_SEQINFO REG_AFE_SEQ2INFO
#define DAC_PULSE_REG_AFE_SEQINFO_ADDR BITP_AFE_SEQ2INFO_ADDR
#define DAC_PULSE_REG_AFE_SEQINFO_LEN BITP_AFE_SEQ2INFO_LEN

static SEQInfo_Type _DAC_seq_info = {
    .SeqId = SEQID_1,
    .WriteSRAM = bFALSE,
};

static SEQInfo_Type _DAC_pulse_seq_info = {
    .SeqId = SEQID_2,
    .WriteSRAM = bFALSE,
};

#define E_MAX(E_begin, E_end, E_pulse_real) (((E_begin > E_end) ? E_begin : E_end) + ((E_pulse_real > 0) ? E_pulse_real : 0))
#define E_STEP_REAL(E_begin, E_end, E_step) ((E_end > E_begin) ? E_step : -E_step)
static int16_t _get_E_pulse_real(
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *const parameters
)
{
    switch (parameters->inversion_option)
    {
    case AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_NONE:
        return (parameters->E_end > parameters->E_begin) ? parameters->E_pulse : -(parameters->E_pulse);
    
    case AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_BOTH:
        return (parameters->E_end > parameters->E_begin) ? (-parameters->E_pulse) : parameters->E_pulse;
    
    case AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_CATHODIC:
        return parameters->E_pulse;
    
    case AD5940_ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_ANODIC:
        return (-parameters->E_pulse);
    }
    return 0;
}
#define STEP_NUMBER(E_begin, E_end, E_step) (abs(((E_end - E_begin) / E_step) * 2) + 2)
#define T_INTERVAL(E_step, scan_rate) (1E3F * ((float) (E_step)) / ((float) scan_rate))

static AD5940Err _check_parameters(
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *const parameters
)
{
    if(parameters->E_step == 0) return AD5940ERR_PARA;
    if(parameters->t_pulse >= T_INTERVAL(
        parameters->E_step, 
        parameters->scan_rate
    )) return AD5940ERR_PARA;
    if(!AD5940_ELECTROCHEMICAL_DPV_is_INVERSION_OPTION(parameters->inversion_option)) return AD5940ERR_PARA;
    return AD5940ERR_OK;
}

AD5940Err AD5940_ELECTROCHEMICAL_DPV_get_times(
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    uint32_t *const times,
    uint16_t *const times_length,
    const uint16_t times_max_length
)
{
    AD5940Err error = _check_parameters(parameters);
    if(error != AD5940ERR_OK) return error;

    uint16_t step_number = STEP_NUMBER(
        parameters->E_begin, 
        parameters->E_end,
        parameters->E_step
    );
    *times_length = step_number / 2;
    if(times_max_length < *times_length) return AD5940ERR_PARA;
    uint16_t t_interval = T_INTERVAL(
        parameters->E_step, 
        parameters->scan_rate
    );
    for(size_t i=0; i<*times_length; i++)
    {
        times[i] = t_interval * i;
    }
    return AD5940ERR_OK;
}

AD5940Err AD5940_ELECTROCHEMICAL_DPV_get_voltages(
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    int32_t *const voltages,
    uint16_t *const voltages_length,
    const uint16_t voltages_max_length
)
{
    AD5940Err error = _check_parameters(parameters);
    if(error != AD5940ERR_OK) return error;

    const int16_t E_step_real = E_STEP_REAL(
        parameters->E_begin, 
        parameters->E_end, 
        parameters->E_step
    );

	voltages[0] = parameters->E_begin;
    *voltages_length = STEP_NUMBER(
        parameters->E_begin, 
        parameters->E_end, 
        parameters->E_step
    ) / 2;
    if(voltages_max_length < *voltages_length) return AD5940ERR_PARA;
	for(size_t i=0; i<(*voltages_length)-1; i++)
	{
	    voltages[i+1] = voltages[i] + E_step_real;
	}
    return AD5940ERR_OK;
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
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *const parameters
)
{
    AD5940Err error = AD5940ERR_OK;

    error = _check_parameters(parameters);
    if(error != AD5940ERR_OK) return error;

    const int16_t E_step_real = E_STEP_REAL(
        parameters->E_begin, 
        parameters->E_end, 
        parameters->E_step
    );
    const int16_t E_pulse_real = _get_E_pulse_real(
        parameters
    );

	#define SEQLEN_ONESTEP 3L  /* How many sequence commands are needed to update LPDAC. */

	uint32_t SeqCmdBuff[SEQLEN_ONESTEP];

	_DAC_seq_info.SeqRamAddr = start_address;
	_DAC_pulse_seq_info.SeqRamAddr = start_address + SEQLEN_ONESTEP;

	BoolFlag is_pulse = bFALSE;
	int16_t current_E = parameters->E_begin;
	uint32_t current_address = start_address;
    uint32_t lpdac_dat_bit;
    const size_t step_number = STEP_NUMBER(
        parameters->E_begin, 
        parameters->E_end, 
        parameters->E_step
    );
    const int16_t E_max = E_MAX(
        parameters->E_begin, 
        parameters->E_end, 
        E_pulse_real
    );
	for(size_t i=0; i<step_number; i++)
	{
        error = AD5940_UTILITY_get_LPDACDATBIT(
            E_max, 
            current_E,
            &lpdac_dat_bit
        );
        if(error != AD5940ERR_OK) return error;
		SeqCmdBuff[0] = SEQ_WR(REG_AFE_LPDACDAT0, lpdac_dat_bit);
		SeqCmdBuff[1] = SEQ_WAIT(10); /* !!!NOTE LPDAC need 10 clocks to update data. Before send AFE to sleep state, wait 10 extra clocks */
        if(is_pulse)
        {
            SeqCmdBuff[2] = SEQ_WR(
                DAC_REG_AFE_SEQINFO,
                ((current_address + SEQLEN_ONESTEP) << DAC_REG_AFE_SEQINFO_ADDR) 
                | (SEQLEN_ONESTEP << DAC_REG_AFE_SEQINFO_LEN)
            );
        }
        else {
            SeqCmdBuff[2] = SEQ_WR(
                DAC_PULSE_REG_AFE_SEQINFO, 
                ((current_address + SEQLEN_ONESTEP) << DAC_PULSE_REG_AFE_SEQINFO_ADDR) 
                | (SEQLEN_ONESTEP << DAC_PULSE_REG_AFE_SEQINFO_LEN)
            );
        }
        // SeqCmdBuff[3] = SEQ_NOP();   /* Stop sequencer. */
        // SeqCmdBuff[3] = SEQ_SLP();   /* Stop sequencer. */
		AD5940_SEQCmdWrite(current_address, SeqCmdBuff, SEQLEN_ONESTEP);
		is_pulse = (is_pulse == bTRUE) ? bFALSE : bTRUE;
        current_E += (is_pulse == bTRUE) ? E_pulse_real : (-(E_pulse_real - E_step_real));
        current_address += SEQLEN_ONESTEP;
	}
    SeqCmdBuff[0] = SEQ_STOP();   /* Stop sequencer. */
    /* Disable sequencer, END of sequencer interrupt is generated. */
    AD5940_SEQCmdWrite(current_address, SeqCmdBuff, 1);
    *sequence_length = (current_address - start_address) + 1;

    AD5940_WriteReg(
        DAC_REG_AFE_SEQINFO, 
        (_DAC_seq_info.SeqRamAddr << DAC_REG_AFE_SEQINFO_ADDR) 
        | (SEQLEN_ONESTEP << DAC_REG_AFE_SEQINFO_LEN)
    );
    AD5940_WriteReg(
        DAC_PULSE_REG_AFE_SEQINFO, 
        (_DAC_pulse_seq_info.SeqRamAddr << DAC_PULSE_REG_AFE_SEQINFO_ADDR) 
        | (SEQLEN_ONESTEP << DAC_PULSE_REG_AFE_SEQINFO_LEN)
    );

	return AD5940ERR_OK;
}

static AD5940Err _write_sequence_commands(
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    const ADCFilterCfg_Type *const adc_filter,
    const DFTCfg_Type *const dft,
    const AD5940_UTILITY_ClockConfig *const clock,
    const uint32_t DataType
)
{
    AD5940Err error = AD5940ERR_OK;

    uint32_t sequence_address = 0x00;
    uint32_t sequence_commands_length = 0;

    error = AD5940_ELECTROCHEMICAL_UTILITY_write_sequence_commands_config(
        &sequence_address,
        adc_filter,
        dft,
        clock,
        DataType
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

static AD5940Err _start_wakeup_timer_sequence(
    const AD5940_ELECTROCHEMICAL_DPV_PARAMETERS *const parameters,
    const uint32_t FifoSrc, 
    const float LFOSC_frequency
)
{
    /* Configure FIFO and Sequencer for normal Amperometric Measurement */
    AD5940_FIFOThrshSet(STEP_NUMBER(
        parameters->E_begin, 
        parameters->E_end, 
        parameters->E_step
    ));
    AD5940_FIFOCtrlS(FifoSrc, bTRUE);

    AD5940_SEQCtrlS(bTRUE);

    const uint16_t t_interval = T_INTERVAL(
        parameters->E_step, 
        parameters->scan_rate
    );

    #define SAMPLE_DELAY 25

    SEQInfo_Type *ADC_seq_info;
    AD5940_ELECTROCHEMICAL_UTILITY_get_ADC_seq_info(
        &ADC_seq_info
    );

    /* Configure Wakeup Timer*/
	WUPTCfg_Type wupt_cfg;
	wupt_cfg.WuptEn = bTRUE;
	wupt_cfg.WuptEndSeq = WUPTENDSEQ_D;
	wupt_cfg.WuptOrder[0] = _DAC_seq_info.SeqId;
	wupt_cfg.WuptOrder[1] = ADC_seq_info->SeqId;
	wupt_cfg.WuptOrder[2] = _DAC_pulse_seq_info.SeqId;
	wupt_cfg.WuptOrder[3] = ADC_seq_info->SeqId;
	wupt_cfg.SeqxSleepTime[ADC_seq_info->SeqId] = 1;        // The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock.
	wupt_cfg.SeqxWakeupTime[ADC_seq_info->SeqId] = (uint32_t)(LFOSC_frequency * SAMPLE_DELAY * 1E-3F) - 1;
	wupt_cfg.SeqxSleepTime[_DAC_seq_info.SeqId] = 1;        // The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock.
	wupt_cfg.SeqxWakeupTime[_DAC_seq_info.SeqId] = (uint32_t)(LFOSC_frequency * (parameters->t_pulse - SAMPLE_DELAY) * 1E-3F) - 1;
	wupt_cfg.SeqxSleepTime[_DAC_pulse_seq_info.SeqId] = 1;  // The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock.
	wupt_cfg.SeqxWakeupTime[_DAC_pulse_seq_info.SeqId] = (uint32_t)(LFOSC_frequency * (t_interval - parameters->t_pulse - SAMPLE_DELAY) * 1E-3F) - 1;
    AD5940_WUPTCfg(&wupt_cfg);

    return AD5940ERR_OK;
}

AD5940Err AD5940_ELECTROCHEMICAL_DPV_start_with_LPTIA(
    const AD5940_ELECTROCHEMICAL_DPV_LPTIA_CONFIG *const config
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
        config->parameters->E_begin,
        E_MAX(
            config->parameters->E_begin, 
            config->parameters->E_end,
            _get_E_pulse_real(
                config->parameters
            )
        ),
        config->utility_AFERefCfg_Type
    );
    if(error != AD5940ERR_OK) return error;

    error = _write_sequence_commands(
        config->parameters,
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
    AD5940_UTILITY_set_INTCCfg_by_AGPIOCfg_Type(&agpio_cfg, AFEINTSRC_DATAFIFOTHRESH | AFEINTSRC_ENDSEQ);
    AD5940_AGPIOCfg(&agpio_cfg);

    error = _start_wakeup_timer_sequence(
        config->parameters,
        config->FifoSrc,
        config->LFOSC_frequency
    );
    if(error != AD5940ERR_OK) return error;

    return error;
}

AD5940Err AD5940_ELECTROCHEMICAL_DPV_start(
    const AD5940_ELECTROCHEMICAL_DPV_CONFIG *const config
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
        config->parameters->E_begin,
        E_MAX(
            config->parameters->E_begin, 
            config->parameters->E_end,
            _get_E_pulse_real(
                config->parameters
            )
        ),
        config->utility_AFERefCfg_Type
    );
    if(error != AD5940ERR_OK) return error;

    error = _write_sequence_commands(
        config->parameters,
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
    AD5940_UTILITY_set_INTCCfg_by_AGPIOCfg_Type(&agpio_cfg, AFEINTSRC_DATAFIFOTHRESH | AFEINTSRC_ENDSEQ);
    AD5940_AGPIOCfg(&agpio_cfg);

    error = _start_wakeup_timer_sequence(
        config->parameters,
        config->FifoSrc,
        config->LFOSC_frequency
    );
    if(error != AD5940ERR_OK) return error;

    return error;
}

AD5940Err AD5940_ELECTROCHEMICAL_DPV_stop(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count
)
{
    return AD5940_ELECTROCHEMICAL_DPV_interrupt(
        MCU_FIFO_buffer, 
        MCU_FIFO_buffer_max_length,
        AD5940_FIFO_count
    );
}

/**
 * This function is based on the example in the AppCHRONOAMPISR() function found in
 * ad5940-examples/examples/AD5940_ChronoAmperometric/ChronoAmperometric.c. 
 */
AD5940Err AD5940_ELECTROCHEMICAL_DPV_interrupt(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count
)
{
    /* Wakeup AFE by read register, read 10 times at most */
    if(AD5940_WakeUp(10) > 10) return AD5940ERR_WAKEUP;  /* Wakeup Failed */

    AD5940_SleepKeyCtrlS(SLPKEY_LOCK);  /* We need time to read data from FIFO, so, do not let AD5940 goes to hibernate automatically */

    *AD5940_FIFO_count = AD5940_FIFOGetCnt();
    if(*AD5940_FIFO_count > MCU_FIFO_buffer_max_length) return AD5940ERR_BUFF;
    AD5940_FIFORd(MCU_FIFO_buffer, *AD5940_FIFO_count);

    AD5940_INTCClrFlag(AFEINTSRC_DATAFIFOTHRESH);

    AD5940_SleepKeyCtrlS(SLPKEY_UNLOCK); /* Unlock so sequencer can put AD5940 to sleep */

    AD5940_UTILITY_shutdown();
    
    return AD5940ERR_OK;
}

AD5940Err AD5940_ELECTROCHEMICAL_DPV_convert_ADCs_to_currents(
    int32_t *const currents, 
    uint16_t *const currents_length,
    const uint16_t currents_max_length,
    const uint32_t *const adc_data, 
    const uint16_t adc_data_length, 
    const fImpPol_Type *const RtiaCalValue, 
    const uint32_t ADC_PGA_gain, 
    const float ADC_reference_volt
)
{
    AD5940Err error;
    error = AD5940_UTILITY_convert_ADCs_to_currents(
        currents, 
        adc_data, 
        adc_data_length, 
        RtiaCalValue, 
        ADC_PGA_gain, 
        ADC_reference_volt
    );
    *currents_length = adc_data_length / 2;
    if(currents_max_length < (*currents_length)) return AD5940ERR_PARA;
    for(size_t i=0; i<(*currents_length); i++)
    {
        currents[i] = currents[2*i+1] - currents[2*i];
    }
    if(error != AD5940ERR_OK) return error;
    return error;
}
