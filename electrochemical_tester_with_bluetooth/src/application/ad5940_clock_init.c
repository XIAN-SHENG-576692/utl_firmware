#include "ad5940_clock_init.h"

#include "ad5940_hardware.h"

AD5940Err ad5940_clock_init(AD5940_CLOCK_INIT_CTX *ctx)
{
	AD5940Err error = AD5940ERR_OK;

    /* Wakeup AFE by read register, read 10 times at most */
    if(AD5940_WakeUp(10) > 10) return AD5940ERR_WAKEUP;  /* Wakeup Failed */

	// set Active power
	AD5940_set_active_power(
		AFEPWR_LP,
		0x00,
		&ctx->clockConfig
	);

	// cal LFOSC
	LFOSCMeasure_Type lfosc_measure = {
		.CalDuration = 1000,
		.CalSeqAddr = 0x00000000,
		.SystemClkFreq = ctx->clockConfig.SysClkFreq,
	};
	AD5940_LFOSCMeasure(
		&lfosc_measure,
		&ctx->lfoscFrequency
	);

	return error;
}
