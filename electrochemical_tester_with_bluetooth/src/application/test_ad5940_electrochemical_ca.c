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

static AD5940_ELECTROCHEMICAL_CA_LPDAC_LPTIA_CONFIG _config_with_LPDAC_LPTIA;
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

	_config_with_LPDAC_LPTIA = (AD5940_ELECTROCHEMICAL_CA_LPDAC_LPTIA_CONFIG) {
		.parameters = &parameters->ad5940_parameters,
		.fifo_thresh = FIFO_THRESH(parameters->ad5940_parameters.t_interval, parameters->t_run),
		.agpio_cfg = &AD5940_EXTERNAL_agpio_cfg,
		.clock = &test_ad5940_parameters_clockConfig,
		.DataType = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DataType,
		.FifoSrc = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_FifoSrc,
		.LFOSC_frequency = test_ad5940_LFOSC_frequency,
		.afe_ref_cfg = &utl_ad5940_electrochemical_utility_AFERefCfg_Type,
		.lpdac_cfg = &utl_ad5940_electrochemical_utility_LPPACfg_Type,
		.lptia_cfg = &utl_ad5940_electrochemical_utility_LPTIACfg_Type,
		.dsp_cfg = &utl_ad5940_electrochemical_utility_DSPCfg_Type,
	};

	error = AD5940_ELECTROCHEMICAL_CA_start_with_LPDAC_LPTIA(
		&_config_with_LPDAC_LPTIA
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

static AD5940_ELECTROCHEMICAL_CA_LPDAC_HSTIA_CONFIG _config_with_LPDAC_HSTIA;
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

	_config_with_LPDAC_HSTIA = (AD5940_ELECTROCHEMICAL_CA_LPDAC_HSTIA_CONFIG) {
		.parameters = &parameters->ad5940_parameters,
		.fifo_thresh = FIFO_THRESH(parameters->ad5940_parameters.t_interval, parameters->t_run),
		.electrode_routing = electrode_routing,
		.agpio_cfg = &AD5940_EXTERNAL_agpio_cfg,
		.clock = &test_ad5940_parameters_clockConfig,
		.DataType = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_DataType,
		.FifoSrc = UTL_AD5940_ELECTROCHEMICAL_PARAMETERS_FifoSrc,
		.LFOSC_frequency = test_ad5940_LFOSC_frequency,
		.afe_ref_cfg = &utl_ad5940_electrochemical_utility_AFERefCfg_Type,
		.lpdac_cfg = &utl_ad5940_electrochemical_utility_LPPACfg_Type,
		.hstia_cfg = &utl_ad5940_electrochemical_utility_HSTIACfg_Type,
		.dsp_cfg = &utl_ad5940_electrochemical_utility_DSPCfg_Type,
	};

	error = AD5940_ELECTROCHEMICAL_CA_start_with_LPDAC_HSTIA(
		&_config_with_LPDAC_HSTIA
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
