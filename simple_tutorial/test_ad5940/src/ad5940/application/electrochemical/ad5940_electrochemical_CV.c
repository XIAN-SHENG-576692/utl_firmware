#include "ad5940_electrochemical_CV.h"

#include "ad5940.h"
#include "ad5940_utility.h"

#include "ad5940_electrochemical_utility.h"

#include <stdlib.h>

#define DAC0_REG_AFE_SEQINFO REG_AFE_SEQ1INFO
#define DAC0_REG_AFE_SEQINFO_ADDR BITP_AFE_SEQ1INFO_ADDR
#define DAC0_REG_AFE_SEQINFO_LEN BITP_AFE_SEQ1INFO_LEN

#define DAC1_REG_AFE_SEQINFO REG_AFE_SEQ2INFO
#define DAC1_REG_AFE_SEQINFO_ADDR BITP_AFE_SEQ2INFO_ADDR
#define DAC1_REG_AFE_SEQINFO_LEN BITP_AFE_SEQ2INFO_LEN

static SEQInfo_Type _DAC0_seq_info = {
    .SeqId = SEQID_1,
    .WriteSRAM = bFALSE,
};

static SEQInfo_Type _DAC1_seq_info = {
    .SeqId = SEQID_2,
    .WriteSRAM = bFALSE,
};

#define E_MAX_VERTEX(E_vertex1, E_vertex2) ((E_vertex1 > E_vertex2) ? E_vertex1 : E_vertex2)
#define E_MAX(E_begin, E_vertex1, E_vertex2) ((E_begin > E_MAX_VERTEX(E_vertex1, E_vertex2)) ? E_begin : E_MAX_VERTEX(E_vertex1, E_vertex2))
#define E_STEP_REAL(E_begin, E_end, E_step) ((E_end > E_begin) ? E_step : -E_step)
#define STEP_NUMBER_RAMP(E_begin, E_end, E_step) (abs(E_end - E_begin) / E_step)
#define STEP_NUMBER(E_begin, E_vertex1, E_vertex2, E_step) ((abs(E_vertex1 - E_begin) + abs(E_vertex2 - E_vertex1) + abs(E_vertex2 - E_begin)) / E_step)
#define T_INTERVAL(E_step, scan_rate) (1E3F * ((float) (E_step)) / ((float) scan_rate))

static AD5940Err _check_parameters(
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters
)
{
    if(parameters->E_step == 0) return AD5940ERR_PARA;
    if(parameters->E_begin > parameters->E_vertex1 && parameters->E_begin > parameters->E_vertex2) return AD5940ERR_PARA;
    if(parameters->E_begin < parameters->E_vertex1 && parameters->E_begin < parameters->E_vertex2) return AD5940ERR_PARA;
    return AD5940ERR_OK;
}

AD5940Err AD5940_ELECTROCHEMICAL_CV_get_times(
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint16_t number_of_scans,
    uint32_t *const times,
    uint16_t *const times_length,
    const uint16_t times_max_length
) 
{
    AD5940Err error = _check_parameters(parameters);
    if(error != AD5940ERR_OK) return error;

    uint16_t step_number = STEP_NUMBER(
        parameters->E_begin, 
        parameters->E_vertex1, 
        parameters->E_vertex2, 
        parameters->E_step
    );
    *times_length = step_number * number_of_scans + 1;
    if(times_max_length < *times_length) return AD5940ERR_PARA;
    uint16_t t_interval = T_INTERVAL(
        parameters->E_step, 
        parameters->scan_rate
    );
    for(size_t i=0; i<number_of_scans; i++)
    {
        for(size_t j=0; j<step_number; j++)
        {
            times[((i * step_number) + j)] = t_interval * ((i * step_number) + j);
        }
    }
    times[(*times_length) - 1] = t_interval * (number_of_scans * step_number);
    return AD5940ERR_OK;
}

AD5940Err AD5940_ELECTROCHEMICAL_CV_get_voltages(
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint16_t number_of_scans,
    int32_t *const voltages,
    uint16_t *const voltages_length,
    const uint16_t voltages_max_length
) 
{
    AD5940Err error = _check_parameters(parameters);
    if(error != AD5940ERR_OK) return error;

    if(number_of_scans == 0) return AD5940ERR_PARA;

    uint16_t step_number = STEP_NUMBER(
        parameters->E_begin, 
        parameters->E_vertex1, 
        parameters->E_vertex2, 
        parameters->E_step
    );
    *voltages_length = step_number * number_of_scans + 1;
    if(voltages_max_length < *voltages_length) return AD5940ERR_PARA;
    
    int16_t E_step_real;
    uint16_t step_number_ramp;
    int16_t index = 0;
    int32_t current_E;
	for(size_t i=0; i<3; i++)
	{
        switch (i)
        {
        case 0:
	        current_E = parameters->E_begin;
            E_step_real = E_STEP_REAL(
                parameters->E_begin, 
                parameters->E_vertex1, 
                parameters->E_step
            );
            step_number_ramp = STEP_NUMBER_RAMP(
                parameters->E_begin, 
                parameters->E_vertex1, 
                parameters->E_step
            );
            break;
        case 1:
	        current_E = parameters->E_vertex1;
            E_step_real = E_STEP_REAL(
                parameters->E_vertex1, 
                parameters->E_vertex2, 
                parameters->E_step
            );
            step_number_ramp = STEP_NUMBER_RAMP(
                parameters->E_vertex1, 
                parameters->E_vertex2, 
                parameters->E_step
            );
            break;
        case 2:
	        current_E = parameters->E_vertex2;
            E_step_real = E_STEP_REAL(
                parameters->E_vertex2, 
                parameters->E_begin, 
                parameters->E_step
            );
            step_number_ramp = STEP_NUMBER_RAMP(
                parameters->E_vertex2, 
                parameters->E_begin, 
                parameters->E_step
            );
            break;
        default:
            break;
        }
        for(size_t j=0; j<step_number_ramp; j++)
        {
            voltages[index] = current_E;
            index++;
            current_E += E_step_real;
        }
	}

    voltages[index] = voltages[0];
    index++;

    for(size_t i=1; i<number_of_scans; i++)
	{
        for(size_t j=0; j<step_number; j++)
        {
            voltages[index] = voltages[j + 1];
            index++;
        }
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
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters
)
{
    AD5940Err error = AD5940ERR_OK;

    error = _check_parameters(parameters);
    if(error != AD5940ERR_OK) return error;

	#define SEQLEN_ONESTEP 3L  /* How many sequence commands are needed to update LPDAC. */

	uint32_t SeqCmdBuff[SEQLEN_ONESTEP];

	_DAC0_seq_info.SeqRamAddr = start_address;
	_DAC1_seq_info.SeqRamAddr = start_address + SEQLEN_ONESTEP;

    const int16_t E_max = E_MAX_VERTEX(
        parameters->E_vertex1, 
        parameters->E_vertex2
    );
    int16_t E_step_real;

	BoolFlag seq_selection = bFALSE;
	uint32_t current_address = start_address;
    uint32_t lpdac_dat_bit;

	int16_t current_E;
    size_t step_number;
	for(size_t i=0; i<3; i++)
	{
        switch (i)
        {
        case 0:
	        current_E = parameters->E_begin;
            E_step_real = E_STEP_REAL(
                parameters->E_begin, 
                parameters->E_vertex1, 
                parameters->E_step
            );
            step_number = STEP_NUMBER_RAMP(
                parameters->E_begin, 
                parameters->E_vertex1, 
                parameters->E_step
            );
            break;
        case 1:
	        current_E = parameters->E_vertex1;
            E_step_real = E_STEP_REAL(
                parameters->E_vertex1, 
                parameters->E_vertex2, 
                parameters->E_step
            );
            step_number = STEP_NUMBER_RAMP(
                parameters->E_vertex1, 
                parameters->E_vertex2, 
                parameters->E_step
            );
            break;
        case 2:
	        current_E = parameters->E_vertex2;
            E_step_real = E_STEP_REAL(
                parameters->E_vertex2, 
                parameters->E_begin, 
                parameters->E_step
            );
            step_number = STEP_NUMBER_RAMP(
                parameters->E_vertex2, 
                parameters->E_begin, 
                parameters->E_step
            );
            break;
        default:
            break;
        }
        for(size_t j=0; j<step_number; j++)
        {
            error = AD5940_UTILITY_get_LPDACDATBIT(
                E_max, 
                current_E,
                &lpdac_dat_bit
            );
            if(error != AD5940ERR_OK) return error;
            SeqCmdBuff[0] = SEQ_WR(REG_AFE_LPDACDAT0, lpdac_dat_bit);
            SeqCmdBuff[1] = SEQ_WAIT(10); /* !!!NOTE LPDAC need 10 clocks to update data. Before send AFE to sleep state, wait 10 extra clocks */
            if(seq_selection)
            {
                if(i == 2 && j == (step_number - 1)) 
                {
                    SeqCmdBuff[2] = SEQ_WR(
                        DAC0_REG_AFE_SEQINFO,
                        (start_address << DAC0_REG_AFE_SEQINFO_ADDR) 
                        | (SEQLEN_ONESTEP << DAC0_REG_AFE_SEQINFO_LEN)
                    );
                }
                else
                {
                    SeqCmdBuff[2] = SEQ_WR(
                        DAC0_REG_AFE_SEQINFO,
                        ((current_address + SEQLEN_ONESTEP) << DAC0_REG_AFE_SEQINFO_ADDR) 
                        | (SEQLEN_ONESTEP << DAC0_REG_AFE_SEQINFO_LEN)
                    );
                }
            }
            else {
                if(i == 2 && j == (step_number - 1)) 
                {
                    SeqCmdBuff[2] = SEQ_WR(
                        DAC1_REG_AFE_SEQINFO,
                        (start_address << DAC1_REG_AFE_SEQINFO_ADDR) 
                        | (SEQLEN_ONESTEP << DAC1_REG_AFE_SEQINFO_LEN)
                    );
                }
                else
                {
                    SeqCmdBuff[2] = SEQ_WR(
                        DAC1_REG_AFE_SEQINFO,
                        ((current_address + SEQLEN_ONESTEP) << DAC1_REG_AFE_SEQINFO_ADDR) 
                        | (SEQLEN_ONESTEP << DAC1_REG_AFE_SEQINFO_LEN)
                    );
                }
            }
            AD5940_SEQCmdWrite(current_address, SeqCmdBuff, SEQLEN_ONESTEP);
            seq_selection = (seq_selection == bTRUE) ? bFALSE : bTRUE;
            current_E += E_step_real;
            current_address += SEQLEN_ONESTEP;
        }
	}
    *sequence_length = current_address - start_address;

    AD5940_WriteReg(
        DAC0_REG_AFE_SEQINFO, 
        (_DAC0_seq_info.SeqRamAddr << DAC0_REG_AFE_SEQINFO_ADDR) 
        | (SEQLEN_ONESTEP << DAC0_REG_AFE_SEQINFO_LEN)
    );
    AD5940_WriteReg(
        DAC1_REG_AFE_SEQINFO, 
        (_DAC1_seq_info.SeqRamAddr << DAC1_REG_AFE_SEQINFO_ADDR) 
        | (SEQLEN_ONESTEP << DAC1_REG_AFE_SEQINFO_LEN)
    );

	return AD5940ERR_OK;
}

static AD5940Err _write_sequence_commands(
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const ADCFilterCfg_Type *const adc_filter,
    const DFTCfg_Type *const dft,
    const AD5940_UTILITY_ClockConfig *const clock,
    uint32_t DataType
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

static uint32_t _FIFOThresh = 0;
static BoolFlag _first = bFALSE;

static AD5940Err _start_wakeup_timer_sequence(
    const AD5940_ELECTROCHEMICAL_CV_PARAMETERS *const parameters,
    const uint32_t FifoSrc, 
    const float LFOSC_frequency
)
{
    const uint16_t t_interval = T_INTERVAL(
        parameters->E_step, 
        parameters->scan_rate
    );
    _FIFOThresh = STEP_NUMBER(
        parameters->E_begin, 
        parameters->E_vertex1, 
        parameters->E_vertex2, 
        parameters->E_step
    ) + 1;

    /* Configure FIFO and Sequencer for normal Amperometric Measurement */
    _first = bTRUE;
    AD5940_FIFOThrshSet(_FIFOThresh);
    AD5940_FIFOCtrlS(FifoSrc, bTRUE);

    AD5940_SEQCtrlS(bTRUE);

    #define SAMPLE_DELAY 25
    
    SEQInfo_Type *ADC_seq_info;
    AD5940_ELECTROCHEMICAL_UTILITY_get_ADC_seq_info(
        &ADC_seq_info
    );

    /* Configure Wakeup Timer*/
	WUPTCfg_Type wupt_cfg;
	wupt_cfg.WuptEn = bTRUE;
	wupt_cfg.WuptEndSeq = WUPTENDSEQ_D;
	wupt_cfg.WuptOrder[0] = _DAC0_seq_info.SeqId;
	wupt_cfg.WuptOrder[1] = ADC_seq_info->SeqId;
	wupt_cfg.WuptOrder[2] = _DAC1_seq_info.SeqId;
	wupt_cfg.WuptOrder[3] = ADC_seq_info->SeqId;
	wupt_cfg.SeqxSleepTime[ADC_seq_info->SeqId] = 1;        // The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock.
	wupt_cfg.SeqxWakeupTime[ADC_seq_info->SeqId] = (uint32_t)(LFOSC_frequency * SAMPLE_DELAY * 1E-3F) - 1;
	wupt_cfg.SeqxSleepTime[_DAC0_seq_info.SeqId] = 1;        // The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock.
	wupt_cfg.SeqxWakeupTime[_DAC0_seq_info.SeqId] = (uint32_t)(LFOSC_frequency * (t_interval - SAMPLE_DELAY) * 1E-3F) - 1;
	wupt_cfg.SeqxSleepTime[_DAC1_seq_info.SeqId] = 1;  // The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock.
	wupt_cfg.SeqxWakeupTime[_DAC1_seq_info.SeqId] = (uint32_t)(LFOSC_frequency * (t_interval - SAMPLE_DELAY) * 1E-3F) - 1;
    AD5940_WUPTCfg(&wupt_cfg);

    return AD5940ERR_OK;
}

AD5940Err AD5940_ELECTROCHEMICAL_CV_start_with_LPTIA(
    const AD5940_ELECTROCHEMICAL_CV_LPTIA_CONFIG *const config
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
        E_MAX_VERTEX(
            config->parameters->E_vertex1, 
            config->parameters->E_vertex2
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

AD5940Err AD5940_ELECTROCHEMICAL_CV_start(
    const AD5940_ELECTROCHEMICAL_CV_CONFIG *const config
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
        E_MAX_VERTEX(
            config->parameters->E_vertex1, 
            config->parameters->E_vertex2
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

AD5940Err AD5940_ELECTROCHEMICAL_CV_stop(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count
)
{
    return AD5940_ELECTROCHEMICAL_CV_interrupt(
        MCU_FIFO_buffer, 
        MCU_FIFO_buffer_max_length,
        AD5940_FIFO_count,
        bFALSE
    );
}

AD5940Err AD5940_ELECTROCHEMICAL_CV_interrupt(
    uint32_t *const MCU_FIFO_buffer, 
    const uint16_t MCU_FIFO_buffer_max_length,
    uint16_t *const AD5940_FIFO_count,
    const BoolFlag isContinue
)
{
    /* Wakeup AFE by read register, read 10 times at most */
    if(AD5940_WakeUp(10) > 10) return AD5940ERR_WAKEUP;  /* Wakeup Failed */

    AD5940_SleepKeyCtrlS(SLPKEY_LOCK);  /* We need time to read data from FIFO, so, do not let AD5940 goes to hibernate automatically */

    *AD5940_FIFO_count = AD5940_FIFOGetCnt();
    if(*AD5940_FIFO_count > MCU_FIFO_buffer_max_length) return AD5940ERR_BUFF;
    AD5940_FIFORd(MCU_FIFO_buffer, *AD5940_FIFO_count);

    if(isContinue == bTRUE)
    {
        if(_first == bTRUE)
        {
            AD5940_FIFOThrshSet(_FIFOThresh - 1);
            _first = bFALSE;
        }
        AD5940_INTCClrFlag(AFEINTSRC_DATAFIFOTHRESH);
    }
    else
    {
        AD5940_SleepKeyCtrlS(SLPKEY_UNLOCK); /* Unlock so sequencer can put AD5940 to sleep */
        AD5940_INTCClrFlag(AFEINTSRC_DATAFIFOTHRESH);
        AD5940_UTILITY_shutdown();
    }
    
    return 0;
}
