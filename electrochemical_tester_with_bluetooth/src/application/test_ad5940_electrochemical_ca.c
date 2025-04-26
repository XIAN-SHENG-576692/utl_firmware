#include "test_ad5940_electrochemical_ca.h"

#include "ad5940_hardware.h"

#include "test_ad5940_lock.h"
#include "test_ad5940_parameters.h"
#include "test_ad5940_electrochemical_parameters.h"
#include "test_ad5940_electrochemical_calibration_result.h"

#include "utl_ad5940_electrochemical_parameters.h"
#include "utl_ad5940_electrochemical_utility_parameters.h"

#include "ad5940_electrochemical_ca.h"

#include <zephyr/logging/log.h>
#define LOG_MODULE_NAME ca
static LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#define FIFO_THRESH(t_interval, t_run) (round(t_run / t_interval) + 1)

static AD5940Err result(
	const fImpPol_Type *const rtia_result,
    const int16_t adc_buffer_max_length,
    uint32_t *const adc_buffer,
    int16_t *const adc_buffer_length
)
{
	AD5940Err error = AD5940ERR_OK;

	test_ad5940_lock_wait();

	error = AD5940_ELECTROCHEMICAL_CA_interrupt(
		adc_buffer_max_length,
		0,
		adc_buffer,
		adc_buffer_length
	);
	if(error != AD5940ERR_OK) return error;

	// error = AD5940_convert_adcs_to_currents(
	// 	test_ad5940_mcu_fifo_buffer,
	// 	currents_length,
	// 	rtia_result,
	// 	UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADCPga, 
	// 	UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_ADC_REFERENCE_VOLT,
	// 	currents
	// );
	// if(error != AD5940ERR_OK) return error;

	return AD5940ERR_OK;
}

AD5940Err test_ad5940_electrochemical_ca_with_LPDAC_LPTIA(
    const ELECTROCHEMICAL_CA *const parameters,
    const int16_t adc_buffer_max_length,
    uint32_t *const adc_buffer,
    int16_t *const adc_buffer_length
)
{
	AD5940Err error = AD5940ERR_OK;
	
	utl_ad5940_electrochemical_utility_HSTIACfg_Type.HstiaRtiaSel = test_ad5940_HstiaRtiaSel;
	utl_ad5940_electrochemical_utility_DSPCfg_Type.ADCFilterCfg.ADCRate = test_ad5940_parameters_clockConfig.ADCRate;
	ad5940_electrochemical_run_config.LFOSC_frequency = test_ad5940_LFOSC_frequency;

	AD5940_ELECTROCHEMICAL_CA_CONFIG _config = {
		.parameters = &parameters->ad5940_parameters,
		.fifo_thresh = FIFO_THRESH(parameters->ad5940_parameters.t_interval, parameters->t_run),
		.run = &ad5940_electrochemical_run_config,
		.lpdac_to_lptia = &ad5940_electrochemical_lpdac_to_lptia_config,
		.lpdac_to_hstia = NULL,
		.hsdac_mmr_to_hstia = NULL,
	};

	error = AD5940_ELECTROCHEMICAL_CA_start(
		&_config
	);
	if(error != AD5940ERR_OK) return error;

	error = result(
		&test_ad5940_hsrtia_result,
		adc_buffer_max_length,
		adc_buffer,
		adc_buffer_length
	);
	if(error != AD5940ERR_OK) return error;

	return error;
}

AD5940Err test_ad5940_electrochemical_ca_with_LPDAC_HSTIA(
    const ELECTROCHEMICAL_CA *const parameters,
    const AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING *const electrode_routing,
    const int16_t adc_buffer_max_length,
    uint32_t *const adc_buffer,
    int16_t *const adc_buffer_length
)
{
	AD5940Err error = AD5940ERR_OK;
	
	utl_ad5940_electrochemical_utility_HSTIACfg_Type.HstiaRtiaSel = test_ad5940_HstiaRtiaSel;
	utl_ad5940_electrochemical_utility_DSPCfg_Type.ADCFilterCfg.ADCRate = test_ad5940_parameters_clockConfig.ADCRate;
	ad5940_electrochemical_run_config.LFOSC_frequency = test_ad5940_LFOSC_frequency;
	ad5940_electrochemical_lpdac_to_hstia_config.electrode_routing = electrode_routing;

	AD5940_ELECTROCHEMICAL_CA_CONFIG _config = {
		.parameters = &parameters->ad5940_parameters,
		.fifo_thresh = FIFO_THRESH(parameters->ad5940_parameters.t_interval, parameters->t_run),
		.run = &ad5940_electrochemical_run_config,
		.lpdac_to_lptia = NULL,
		.lpdac_to_hstia = &ad5940_electrochemical_lpdac_to_hstia_config,
		.hsdac_mmr_to_hstia = NULL,
	};

	error = AD5940_ELECTROCHEMICAL_CA_start(
		&_config
	);
	if(error != AD5940ERR_OK) return error;

	error = result(
		&test_ad5940_hsrtia_result,
		adc_buffer_max_length,
		adc_buffer,
		adc_buffer_length
	);
	if(error != AD5940ERR_OK) return error;

	return error;
}
