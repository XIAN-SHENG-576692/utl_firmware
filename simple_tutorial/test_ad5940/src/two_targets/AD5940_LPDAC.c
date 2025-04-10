/*!
 *****************************************************************************
 @file:    AD5940_LPDAC.c
 @author:  Neo Xu
 @brief:   Low power DAC example.
 -----------------------------------------------------------------------------

Copyright (c) 2017-2019 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.
By using this software you agree to the terms of the associated
Analog Devices Software License Agreement.
 
*****************************************************************************/

/**
 * Note: This example will use LP loop to output voltage on RE0 pin.
 * LPDAC reference: internal 2.5V
 * LP PA(potentialstat amplifier) is used to buffer voltage from Vbias which connects to 12bit LPDAC output
**/

#include "ad5940.h"
#include "AD5940.h"
#include <stdio.h>
#include "string.h"

#include "ad5940_utility_sequence_generator.h"
#include "ad5940_utility_lpdac.h"
#include "ad5940_utility_hsdac.h"

#include "parameters.h"
#include "set_afe.h"
#include "set_hs_loop.h"
#include "set_lp_loop.h"
#include "write_dpv.h"

void AD5940_Main(void)
{
	/* Use hardware reset */
	AD5940_HWReset();
	AD5940_Initialize();
	AD5940_UTILITY_set_sequence_generator_buffer(
		seq_gen_buffer,
		BUFF_SIZE
	);

	set_afe();
	set_lp_loop();
	set_hs_loop();


	AD5940_UTILITY_voltage_to_hsdac_dat_bits(
		0.15,
		EXCITBUFGAIN,
		HSDACGAIN,
		&hsdac_dat_bit
	);

	double v0;
	double v1;
	double v2;
	AD5940_UTILITY_lpdac_dat_6_bit_to_voltage(
		lpdac_dat_6_bit,
		&v0
	);
	AD5940_UTILITY_lpdac_dat_12_bit_to_voltage(
		lpdac_dat_12_bit,
		&v1
	);
	AD5940_UTILITY_hsdac_dat_bits_to_voltage(
		hsdac_dat_bit,
		EXCITBUFGAIN,
		HSDACGAIN,
		&v2
	);
		
	AD5940_LPDAC0WriteS(lpdac_dat_12_bit, lpdac_dat_6_bit);
	AD5940_WGDACCodeS(hsdac_dat_bit);

	while(1);
}

