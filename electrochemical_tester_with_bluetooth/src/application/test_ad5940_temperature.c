#include "test_ad5940_temperature.h"

#include "test_ad5940_lock.h"
#include "test_ad5940_parameters.h"
#include "utl_ad5940_temperature_parameters.h"

AD5940Err test_ad5940_start_temperature(
	int32_t *const temperature
)
{
	AD5940Err error = AD5940ERR_OK;

	utl_ad5940_temperature_start_config.LFOSC_frequency = test_ad5940_LFOSC_frequency;

	error = AD5940_TEMPERATURE_start(
		&utl_ad5940_temperature_start_config
	);
	if(error) return error;

	test_ad5940_lock_wait();

	uint32_t MCU_FIFO_buffer;
	uint16_t fifo_length;
	error = AD5940_TEMPERATURE_interrupt(
		&MCU_FIFO_buffer,
		1,
		0,
		&fifo_length
	);
	if(error) return error;

	error = AD5940_convert_adcs_to_temperatures(
		&MCU_FIFO_buffer,
		fifo_length, 
		TEST_AD5940_TEMPERATURE_ADCPga,
		temperature
	);
	if(error) return error;

	return error;
}
