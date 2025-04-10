#include "ad5940_init.h"

#include "ad5940_hardware.h"

#include "ad5940_lock.h"

#include "ad5940_main.h"

#define AD5940_SEQ_GEN_BUFF 1000
static uint32_t ad5940_seq_gen_buff[AD5940_SEQ_GEN_BUFF];

AD5940Err ad5940_init(void)
{
	AD5940Err err = AD5940ERR_OK;

	ad5940_afeintc0_lock_init();

	err = AD5940_MAIN_init(
		ad5940_seq_gen_buff,
		AD5940_SEQ_GEN_BUFF
	);
	if (err) return err;

    /* Wakeup AFE by read register, read 10 times at most */
    if(AD5940_WakeUp(10) > 10) return AD5940ERR_WAKEUP;  /* Wakeup Failed */

	// set Active power
	// AD5940_set_active_power(
	// 	AFEPWR_LP,
	// 	0x00,
	// 	&test_ad5940_parameters_clockConfig
	// );

	// // cal LFOSC
	// LFOSCMeasure_Type lfosc_measure = {
	// 	.CalDuration = 1000,
	// 	.CalSeqAddr = 0x00000000,
	// 	.SystemClkFreq = test_ad5940_parameters_clockConfig.SysClkFreq,
	// };
	// AD5940_LFOSCMeasure(
	// 	&lfosc_measure,
	// 	&test_ad5940_LFOSC_frequency
	// );

	// utl_ad5940_temperature_start_config.agpio_cfg = &AD5940_EXTERNAL_agpio_cfg;
	// utl_ad5940_temperature_start_config.clock = &test_ad5940_parameters_clockConfig;

	return err;
}
