#include "write_dpv.h"

#include "parameters.h"

#include "ad5940_utility_sequence_generator.h"
#include "ad5940_utility_lpdac.h"
#include "ad5940_utility_hsdac.h"

ELECTROCHEMICAL_ERROR error;

uint16_t lpdac_dat_6_bits;
uint16_t lpdac_dat_12_bits;
uint32_t lpdac_dat_bits;
uint32_t hsdac_dat_bits;

int16_t lpdac_dpv_voltage_max;

uint16_t 
    lpdac_dpv_len,
    hsdac_dpv_len;

int16_t lpdac_voltages_buffer[BUFF_SIZE];
int16_t hsdac_voltages_buffer[BUFF_SIZE];

static ELECTROCHEMICAL_ERROR _init_lp_loop_voltages(void)
{
	ELECTROCHEMICAL_DPV_get_voltage_max(
		&dpv_lp_loop,
		&lpdac_dpv_voltage_max
	);
	AD5940_UTILITY_voltage_to_lpdac_dat_6_bits(
		((double) lpdac_dpv_voltage_max) / ELECTROCHEMICAL_UNIT,
		AD5940_UTILITY_LPDACDAT_HANDLER_CEIL,
		&lpdac_dat_6_bits
	);
	
	error = ELECTROCHEMICAL_DPV_get_voltages(
        &dpv_lp_loop,
        ELECTROCHEMICAL_DPV_ARRAY_OPTION_BOTH,
        BUFF_SIZE,
        lpdac_voltages_buffer,
        &lpdac_dpv_len
	);
	if(error) return error;	
	lpdac_dpv_len;

	return ELECTROCHEMICAL_ERROR_SUCCESS;
}

static ELECTROCHEMICAL_ERROR _init_hs_loop_voltages(void)
{
	ELECTROCHEMICAL_DPV_get_voltage_max(
		&dpv_lp_loop,
		&lpdac_dpv_voltage_max
	);
	AD5940_UTILITY_voltage_to_lpdac_dat_6_bits(
		((double) lpdac_dpv_voltage_max) / ELECTROCHEMICAL_UNIT,
		AD5940_UTILITY_LPDACDAT_HANDLER_CEIL,
		&lpdac_dat_6_bits
	);

	error = ELECTROCHEMICAL_DPV_get_voltages(
        &dpv_hs_loop,
        ELECTROCHEMICAL_DPV_ARRAY_OPTION_STEP,
        BUFF_SIZE,
        hsdac_voltages_buffer,
        &hsdac_dpv_len
	);
	if (error) return error;

	return ELECTROCHEMICAL_ERROR_SUCCESS;
}

#define SEQLEN_ONESTEP 3L  /* How many sequence commands are needed to update DAC. */

static uint32_t _seq_cmd_buffer[10];

ELECTROCHEMICAL_ERROR write_lp_loop_dpv(
	float AdcClkFreq,
	const step_seq_id,
	const pulse_seq_id,
	const uint16_t seq_reg_add,
	uint32_t *const sequence_length
)
{
	error = _init_lp_loop_voltages();
	if(error) return error;	

	uint16_t current_seq_reg_add;
	BoolFlag next_is_pulse;

	current_seq_reg_add = seq_reg_add;

	next_is_pulse = bTRUE;

	// Step
	for(size_t i=0; i<lpdac_dpv_len; i++)
	{
		if(lpdac_dpv_voltage_max > 0) 
		{
			double V_cal;
			error = AD5940_UTILITY_lpdac_dat_6_bits_to_voltage(
				lpdac_dat_6_bits,
				&V_cal
			) - lpdac_dpv_voltage_max;
			AD5940_UTILITY_voltage_to_lpdac_dat_12_bits(
				lpdac_dpv_voltage_max - (((double) lpdac_voltages_buffer[i]) / ELECTROCHEMICAL_UNIT) + V_cal,
				AD5940_UTILITY_LPDACDAT_HANDLER_ROUND,
				&lpdac_dat_12_bits
			);
		}
		else
		{
			AD5940_UTILITY_voltage_to_lpdac_dat_12_bits(
				-(((double) lpdac_voltages_buffer[i]) / ELECTROCHEMICAL_UNIT),
				AD5940_UTILITY_LPDACDAT_HANDLER_ROUND,
				&lpdac_dat_12_bits
			);
		}
		AD5940_UTILITY_combine_lpdac_dat_bits(
			lpdac_dat_6_bits,
			lpdac_dat_12_bits,
			&lpdac_dat_bits
		);
		_seq_cmd_buffer[0] = SEQ_WR(REG_AFE_LPDACDAT0, lpdac_dat_bits);
		
		_seq_cmd_buffer[1] = SEQ_WAIT(AdcClkFreq / 10); /* !!!NOTE LPDAC need 10 clocks to update data. Before send AFE to sleep state, wait 10 extra clocks */

		AD5940_UTILITY_get_change_sequence_info_command(
			(next_is_pulse == bTRUE)
				? pulse_seq_id
				: step_seq_id,
			current_seq_reg_add + SEQLEN_ONESTEP, 
			SEQLEN_ONESTEP,
			_seq_cmd_buffer + 2
		);

		AD5940_AFECtrlS(AFECTRL_ADCPWR | AFECTRL_SINC2NOTCH, bTRUE);
		AD5940_SEQGenInsert(SEQ_WAIT(16*250));  /* wait 250us for reference power up */
		AD5940_AFECtrlS(AFECTRL_ADCCNV, bTRUE);  /* Start ADC convert and DFT */
		AD5940_SEQGenInsert(SEQ_WAIT(WaitClks));  /* wait for first data ready */
		AD5940_AFECtrlS(AFECTRL_ADCPWR | AFECTRL_ADCCNV | AFECTRL_SINC2NOTCH, bFALSE);  /* Stop ADC */

		next_is_pulse = (next_is_pulse == bTRUE) ? bFALSE : bTRUE;
        current_seq_reg_add += SEQLEN_ONESTEP;
	}
    _seq_cmd_buffer[0] = SEQ_STOP();   /* Stop sequencer. */
    /* Disable sequencer, END of sequencer interrupt is generated. */
    AD5940_SEQCmdWrite(current_seq_reg_add, _seq_cmd_buffer, 1);
    *sequence_length = (current_seq_reg_add - seq_reg_add) + 1;

	AD5940_UTILITY_change_sequence_info_command(
		step_seq_id,
		seq_reg_add, 
		SEQLEN_ONESTEP
	);

	AD5940_UTILITY_change_sequence_info_command(
		pulse_seq_id,
		seq_reg_add + SEQLEN_ONESTEP, 
		SEQLEN_ONESTEP
	);

	return AD5940ERR_OK;
}

ELECTROCHEMICAL_ERROR write_dpv(
	const uint16_t seq_reg_add,
	uint32_t *const sequence_length
)
{
	error = _init();
	if(error) return error;	

	uint16_t current_seq_reg_add;
	size_t step_number;
	BoolFlag is_step;

	current_seq_reg_add = seq_reg_add;
	
	step_number = (lpdac_dpv_len > hsdac_dpv_len)
		? lpdac_dpv_len
		: hsdac_dpv_len;

	is_step = bTRUE;

	// Step
	for(size_t i=0; i<step_number; i++)
	{
		uint32_t reg_len = 0;
		if(i < lpdac_dpv_len)
		{
			if(lpdac_dpv_voltage_max > 0) 
			{
				double V_cal;
				error = AD5940_UTILITY_lpdac_dat_6_bits_to_voltage(
					lpdac_dat_6_bits,
					&V_cal
				) - lpdac_dpv_voltage_max;
				AD5940_UTILITY_voltage_to_lpdac_dat_12_bits(
					lpdac_dpv_voltage_max - (((double) lpdac_dpv_ptr[i]) / ELECTROCHEMICAL_UNIT) + V_cal,
					AD5940_UTILITY_LPDACDAT_HANDLER_ROUND,
					&lpdac_dat_12_bits
				);
			}
			else
			{
				AD5940_UTILITY_voltage_to_lpdac_dat_12_bits(
					-(((double) lpdac_dpv_ptr[i]) / ELECTROCHEMICAL_UNIT),
					AD5940_UTILITY_LPDACDAT_HANDLER_ROUND,
					&lpdac_dat_12_bits
				);
			}
			AD5940_UTILITY_combine_lpdac_dat_bits(
				lpdac_dat_6_bits,
				lpdac_dat_12_bits,
				&lpdac_dat_bits
			);
			_seq_cmd_buffer[reg_len] = SEQ_WR(REG_AFE_LPDACDAT0, lpdac_dat_bits);
			reg_len++;
		}
		if(i < hsdac_dpv_len)
		{
			AD5940_UTILITY_voltage_to_hsdac_dat_bits(
				((double) hsdac_dpv_ptr[i]) / ELECTROCHEMICAL_UNIT,
				AD5940_UTILITY_HSDACDAT_HANDLER_CEIL,
				EXCITBUFGAIN,
				HSDACGAIN,
				&hsdac_dat_bits
			);
			_seq_cmd_buffer[reg_len] = SEQ_WR(REG_AFE_HSDACDAT, hsdac_dat_bits);
			reg_len++;
		}
		_seq_cmd_buffer[reg_len] = SEQ_WAIT(10); /* !!!NOTE LPDAC need 10 clocks to update data. Before send AFE to sleep state, wait 10 extra clocks */
		reg_len++;
		
		if(i != 0) 
		{
			AD5940_UTILITY_get_change_sequence_info_command(
				(is_step == bTRUE)
					? DPV_PULSE_SEQ_ID
					: DPV_STEP_SEQ_ID,
				current_seq_reg_add + reg_len + 1, 
				reg_len + 1,
				_seq_cmd_buffer + reg_len
			);
		}

		reg_len++;
		AD5940_UTILITY_get_change_sequence_info_command(
			(is_step == bTRUE)
				? DPV_STEP_SEQ_ID
				: DPV_PULSE_SEQ_ID,
			current_seq_reg_add, 
			reg_len,
			_seq_cmd_buffer
		);

		is_step = (is_step == bTRUE) ? bFALSE : bTRUE;
        current_seq_reg_add += reg_len;

		if(i == 0) first_reg_len = reg_len;
	}
    _seq_cmd_buffer[0] = SEQ_STOP();   /* Stop sequencer. */
    /* Disable sequencer, END of sequencer interrupt is generated. */
    AD5940_SEQCmdWrite(current_seq_reg_add, _seq_cmd_buffer, 1);
    *sequence_length = (current_seq_reg_add - seq_reg_add) + 1;

	change_sequence_info_command(
		DPV_STEP_SEQ_ID,
		seq_reg_add, 
		reg_len + 1,
		_seq_cmd_buffer + reg_len
	);

    AD5940_WriteReg(
        DPV_STEP_SEQ_ID, 
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
