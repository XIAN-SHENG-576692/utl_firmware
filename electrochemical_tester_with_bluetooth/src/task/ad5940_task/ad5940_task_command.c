#include "ad5940_task_command.h"

#include <stdatomic.h>
#include <math.h>

#include "ad5940_task_private.h"

#include "ad5940_temperature.h"

#include "ad5940_electrochemical_ca.h"
#include "ad5940_electrochemical_cv.h"
#include "ad5940_electrochemical_dpv.h"

static const AD5940_TASK_COMMAND_CFG *_cfg;
static volatile _Atomic AD5940_TASK_COMMAND_STATE _state = AD5940_TASK_COMMAND_STATE_UNINITIALIZED;

static AD5940_TASK_MEASUREMENT_PARAM measurement_param;

AD5940_TASK_COMMAND_STATE AD5940_TASK_COMMAND_get_state(void)
{
    return atomic_load(&_state);
}

int AD5940_TASK_COMMAND_measure(
    const AD5940_TASK_MEASUREMENT_PARAM *const param
)
{
    AD5940_TASK_COMMAND_get_access_measurement_param_lock();
    measurement_param = *param;
    AD5940_TASK_COMMAND_release_access_measurement_param_lock();
    AD5940_TASK_COMMAND_trigger_measurement();
    return 0;
}

typedef struct
{
    AD5940_ELECTROCHEMICAL_AFERefCfg_Type _utility_AFERefCfg_Type;
    AD5940_ELECTROCHEMICAL_LPDACfg_Type _utility_LPPACfg_Type;
    AD5940_ELECTROCHEMICAL_LPTIACfg_Type _utility_LPTIACfg_Type;
    AD5940_ELECTROCHEMICAL_HSTIACfg_Type _utility_HSTIACfg_Type;
    AD5940_ELECTROCHEMICAL_DSPCfg_Type _utility_DSPCfg_Type;

    AD5940_ELECTROCHEMICAL_RUN_CONFIG run_config;
    AD5940_ELECTROCHEMICAL_LPDAC_TO_LPTIA_CONFIG lpdac_to_lptia_config;
    AD5940_ELECTROCHEMICAL_LPDAC_TO_HSTIA_CONFIG lpdac_to_hstia_config;
    AD5940_ELECTROCHEMICAL_HSDAC_TO_HSTIA_CONFIG hsdac_to_hstia_config;
} _PARAM_ELECTROCHEMICAL;

typedef struct
{
    AD5940_TEMPERATURE_PARAMETERS parameters;
    AD5940_TEMPERATURE_ANALOG_CONFIG analog_cfg;
    AD5940_TEMPERATURE_RUN_CONFIG run_cfg;
} _PARAM_TEMPERATURE;

typedef struct
{
    _PARAM_ELECTROCHEMICAL electrochemical;
    _PARAM_TEMPERATURE temperature;
} _PARAM;

static _PARAM _param = {};

AD5940Err AD5940_TASK_COMMAND_run(AD5940_TASK_COMMAND_CFG *const cfg)
{
    int err = 0;
    _cfg = cfg;

    // ==================================================
    // Electrochemical Init
    _param.electrochemical._utility_AFERefCfg_Type = (AD5940_ELECTROCHEMICAL_AFERefCfg_Type) {
    };

    _param.electrochemical._utility_LPPACfg_Type = (AD5940_ELECTROCHEMICAL_LPDACfg_Type) {
        .LpAmpPwrMod = _cfg->param.electrochemical.LpAmpPwrMod,
    };

    _param.electrochemical._utility_LPTIACfg_Type = (AD5940_ELECTROCHEMICAL_LPTIACfg_Type) {
        .LpTiaRf = _cfg->param.electrochemical.LpTiaRf,
        .LpTiaRload = _cfg->param.electrochemical.LpTiaRload,
        .LpTiaRtia = _cfg->param.electrochemical.LpTiaRtia,
    };

    _param.electrochemical._utility_HSTIACfg_Type = (AD5940_ELECTROCHEMICAL_HSTIACfg_Type) {
        .DiodeClose = bFALSE,
        .ExtRtia = bFALSE,
        .HstiaCtia = 31,
        .HstiaDeRload = HSTIADERLOAD_OPEN,
        .HstiaDeRtia = HSTIADERTIA_OPEN,
        .HstiaRtiaSel = _cfg->param.electrochemical.HstiaRtiaSel,
    };

    _param.electrochemical._utility_DSPCfg_Type = (AD5940_ELECTROCHEMICAL_DSPCfg_Type) {
        .ADCDigCompCfg = {0},
        .ADCFilterCfg = {
            .ADCAvgNum = _cfg->param.electrochemical.ADCAvgNum,
            .ADCRate = _cfg->param.electrochemical.ADCRate,
            .ADCSinc2Osr = _cfg->param.electrochemical.ADCSinc2Osr,
            .ADCSinc3Osr = _cfg->param.electrochemical.ADCSinc3Osr,
            .BpNotch = _cfg->param.electrochemical.BpNotch,
            .BpSinc3 = _cfg->param.electrochemical.BpSinc3,
            // .DFTClkEnable
            // .Sinc2NotchClkEnable,
            .Sinc2NotchEnable = _cfg->param.electrochemical.Sinc2NotchEnable,
            // .Sinc3ClkEnable
        },
        .ADCPga = _cfg->param.electrochemical.ADCPga,
        .DftCfg = {
            .DftNum = _cfg->param.electrochemical.DftNum,
            .DftSrc = _cfg->param.electrochemical.DftSrc,
            .HanWinEn = _cfg->param.electrochemical.HanWinEn,
        },
        .StatCfg = {0},
    };

    _param.electrochemical.run_config = (AD5940_ELECTROCHEMICAL_RUN_CONFIG) {
        .agpio_cfg = &_cfg->param.common.agpio_cfg,
        .clock_cfg = &_cfg->param.electrochemical.clockConfig,
        .LFOSCClkFreq = _cfg->param.electrochemical.lfoscFrequency,
        .DataType = _cfg->param.electrochemical.DataType,
        .FifoSrc = _cfg->param.electrochemical.FifoSrc,
        .FifoThresh = ADC_SAMPLE_UNIT,
    };

    _param.electrochemical.lpdac_to_lptia_config = (AD5940_ELECTROCHEMICAL_LPDAC_TO_LPTIA_CONFIG) {
        .afe_ref_cfg = &_param.electrochemical._utility_AFERefCfg_Type,
        .dsp_cfg = &_param.electrochemical._utility_DSPCfg_Type,
        .lpdac_cfg = &_param.electrochemical._utility_LPPACfg_Type,
        .lptia_cfg = &_param.electrochemical._utility_LPTIACfg_Type,
    };

    /**
     * You need to set the following parameters:
     * - electrode_routing
     */
    _param.electrochemical.lpdac_to_hstia_config = (AD5940_ELECTROCHEMICAL_LPDAC_TO_HSTIA_CONFIG) {
        .afe_ref_cfg = &_param.electrochemical._utility_AFERefCfg_Type,
        .dsp_cfg = &_param.electrochemical._utility_DSPCfg_Type,
        .lpdac_cfg = &_param.electrochemical._utility_LPPACfg_Type,
        .hstia_cfg = &_param.electrochemical._utility_HSTIACfg_Type,
        .electrode_routing = &measurement_param.param.electrochemical.routing,
    };

    _param.electrochemical.hsdac_to_hstia_config = (AD5940_ELECTROCHEMICAL_HSDAC_TO_HSTIA_CONFIG) {
        
    };

    // ==================================================
    // Temperature Init
    _param.temperature = (_PARAM_TEMPERATURE) {
        .analog_cfg = {
            .ADCAvgNum = _cfg->param.temperature.ADCAvgNum,
            .ADCPga = _cfg->param.temperature.ADCPga,
            .ADCSinc2Osr = _cfg->param.temperature.ADCSinc2Osr,
            .ADCSinc3Osr = _cfg->param.temperature.ADCSinc3Osr,
            .BpNotch = _cfg->param.temperature.BpNotch,
            .BpSinc3 = _cfg->param.temperature.BpSinc3,
            .DataType = _cfg->param.temperature.DataType,
            .FifoSrc = _cfg->param.temperature.FifoSrc,
            .Sinc2NotchEnable = _cfg->param.temperature.Sinc2NotchEnable,
        },
        .parameters = {
        },
        .run_cfg = {
            .agpio_cfg = &_cfg->param.common.agpio_cfg,
            .clock_cfg = &_cfg->param.electrochemical.clockConfig,
            .FIFO_thresh = ADC_SAMPLE_UNIT,
            .LFOSC_frequency = _cfg->param.electrochemical.lfoscFrequency,
        },
    };

    // ==================================================
    // Start
    atomic_store(&_state, AD5940_TASK_COMMAND_STATE_IDLE);

	for (;;) {
        err = AD5940_TASK_COMMAND_wait_measurement();
		if (err) {
            atomic_store(&_state, AD5940_TASK_COMMAND_STATE_ERROR);
            for(;;) {}
        }
        atomic_store(&_state, AD5940_TASK_COMMAND_STATE_EXECUTING);
        
        // callback
        if(_cfg->callback.start != NULL)
        {
            _cfg->callback.start();
        }

        AD5940_TASK_COMMAND_get_access_measurement_param_lock();

        uint16_t adc_length;
        switch (measurement_param.type)
        {
        case AD5940_TASK_TYPE_TEMPERATURE:
        {
            _param.temperature.parameters.sampling_interval = measurement_param.param.temperature.sampling_interval;
            _param.temperature.parameters.TEMPSENS = measurement_param.param.temperature.TEMPSENS;
            AD5940_TEMPERATURE_START_CONFIG _config = {
                .analog_cfg = &_param.temperature.analog_cfg,
                .parameters = &_param.temperature.parameters,
                .run_cfg = &_param.temperature.run_cfg,
            };
            err = AD5940_TEMPERATURE_start(
                &_config
            );
            if(err != AD5940ERR_OK) return err;
            AD5940_TASK_ADC_reset(
                AD5940_TASK_ADC_RESULT_FLAG_TEMPERATURE,
                (uint16_t) round(measurement_param.param.temperature.sampling_time / measurement_param.param.temperature.sampling_interval)
            );
            break;
        }
        case AD5940_TASK_TYPE_ELECTROCHEMICAL_CA: 
        {
            AD5940_ELECTROCHEMICAL_CA_CONFIG _config = {
                .parameters = &measurement_param.param.electrochemical.parameters.ca.ad5940_parameters,
                .run = &_param.electrochemical.run_config,
                .path_type = 1,
                .path.lpdac_to_hstia = &_param.electrochemical.lpdac_to_hstia_config,
            };

            err = AD5940_ELECTROCHEMICAL_CA_start(
                &_config
            );
            if(err != AD5940ERR_OK) return err;

            // ADC length
            #define CA_FIFO_THRESH(t_interval, t_run) (round(t_run / t_interval) + 1)
            adc_length = CA_FIFO_THRESH(
                measurement_param.param.electrochemical.parameters.ca.ad5940_parameters.t_interval, 
                measurement_param.param.electrochemical.parameters.ca.t_run
            );
            AD5940_TASK_ADC_reset(
                AD5940_TASK_ADC_RESULT_FLAG_HSTIA_VOLT_TO_CURRENT,
                adc_length
            );
            break;
        }
        case AD5940_TASK_TYPE_ELECTROCHEMICAL_CV: 
        {
            AD5940_ELECTROCHEMICAL_CV_CONFIG _config = {
                .parameters = &measurement_param.param.electrochemical.parameters.cv.ad5940_parameters,
                .run = &_param.electrochemical.run_config,
                .path_type = 1,
                .path.lpdac_to_hstia = &_param.electrochemical.lpdac_to_hstia_config,
            };

            err = AD5940_ELECTROCHEMICAL_CV_start(
                &_config
            );
            if(err != AD5940ERR_OK) return err;

            // ADC length
            err = AD5940_ELECTROCHEMICAL_CV_get_fifo_count(
                &measurement_param.param.electrochemical.parameters.cv.ad5940_parameters,
                &adc_length
            );
            if(err != AD5940ERR_OK) break;
            adc_length *= measurement_param.param.electrochemical.parameters.cv.number_of_scans;
            AD5940_TASK_ADC_reset(
                AD5940_TASK_ADC_RESULT_FLAG_HSTIA_VOLT_TO_CURRENT,
                adc_length
            );
            break;
        }
        case AD5940_TASK_TYPE_ELECTROCHEMICAL_DPV: 
        {
            AD5940_ELECTROCHEMICAL_DPV_CONFIG _config = {
                .parameters = &measurement_param.param.electrochemical.parameters.dpv.ad5940_parameters,
                .run = &_param.electrochemical.run_config,
                .path_type = 1,
                .path.lpdac_to_hstia = &_param.electrochemical.lpdac_to_hstia_config,
            };

            err = AD5940_ELECTROCHEMICAL_DPV_start(
                &_config
            );
            if(err != AD5940ERR_OK) return err;

            // ADC length
            err = AD5940_ELECTROCHEMICAL_DPV_get_fifo_count(
                &measurement_param.param.electrochemical.parameters.dpv.ad5940_parameters,
                &adc_length
            );
            if(err != AD5940ERR_OK) break;
            AD5940_TASK_ADC_reset(
                AD5940_TASK_ADC_RESULT_FLAG_HSTIA_VOLT_TO_CURRENT,
                adc_length
            );
            break;
        }
        default:
            break;
        }
        
        AD5940_TASK_COMMAND_release_access_measurement_param_lock();

        atomic_store(&_state, AD5940_TASK_COMMAND_STATE_IDLE);

        // callback
        if(_cfg->callback.end != NULL)
        {
            _cfg->callback.end();
        }
	}

    return err;
}
