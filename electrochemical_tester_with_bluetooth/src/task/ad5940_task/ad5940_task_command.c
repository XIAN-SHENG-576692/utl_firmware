#include "ad5940_task_command.h"

#include <stdatomic.h>

#include "ad5940_task_private.h"

#include "ad5940_electrochemical_ca.h"
#include "ad5940_electrochemical_cv.h"
#include "ad5940_electrochemical_dpv.h"

static const AD5940_TASK_COMMAND_CFG *_cfg;
static volatile _Atomic AD5940_TASK_COMMAND_STATE _state = AD5940_TASK_COMMAND_STATE_UNINITIALIZED;

static AD5940_TASK_ELECTROCHEMICAL_TYPE _type;
static AD5940_TASK_ELECTROCHEMICAL_PARAMETERS_UNION _parameters;
static AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING _routing;

AD5940_TASK_COMMAND_STATE AD5940_TASK_COMMAND_get_state(void)
{
    return atomic_load(&_state);
}

int AD5940_TASK_COMMAND_measure(
    const AD5940_TASK_ELECTROCHEMICAL_TYPE type,
    const AD5940_TASK_ELECTROCHEMICAL_PARAMETERS_UNION *const parameters,
    const AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING *const routing
)
{
    AD5940_TASK_COMMAND_get_access_measurement_param_lock();
    _type = type;
    _parameters = *parameters;
    _routing = *routing;
    AD5940_TASK_COMMAND_release_access_measurement_param_lock();
    AD5940_TASK_COMMAND_trigger_measurement();
    return 0;
}

static AD5940_ELECTROCHEMICAL_AFERefCfg_Type _utility_AFERefCfg_Type;
static AD5940_ELECTROCHEMICAL_LPDACfg_Type _utility_LPPACfg_Type;
static AD5940_ELECTROCHEMICAL_LPTIACfg_Type _utility_LPTIACfg_Type;
static AD5940_ELECTROCHEMICAL_HSTIACfg_Type _utility_HSTIACfg_Type;
static AD5940_ELECTROCHEMICAL_DSPCfg_Type _utility_DSPCfg_Type;

static AD5940_ELECTROCHEMICAL_RUN_CONFIG run_config;
static AD5940_ELECTROCHEMICAL_LPDAC_TO_LPTIA_CONFIG lpdac_to_lptia_config;
static AD5940_ELECTROCHEMICAL_LPDAC_TO_HSTIA_CONFIG lpdac_to_hstia_config;
static AD5940_ELECTROCHEMICAL_HSDAC_MMR_TO_HSTIA_CONFIG hsdac_mmr_to_hstia_config;

AD5940Err AD5940_TASK_COMMAND_run(AD5940_TASK_COMMAND_CFG *const cfg)
{
    int err = 0;
    _cfg = cfg;

    _utility_AFERefCfg_Type = (AD5940_ELECTROCHEMICAL_AFERefCfg_Type) {
    };

    _utility_LPPACfg_Type = (AD5940_ELECTROCHEMICAL_LPDACfg_Type) {
        .LpAmpPwrMod = _cfg->param.LpAmpPwrMod,
    };

    _utility_LPTIACfg_Type = (AD5940_ELECTROCHEMICAL_LPTIACfg_Type) {
        .LpTiaRf = _cfg->param.LpTiaRf,
        .LpTiaRload = _cfg->param.LpTiaRload,
        .LpTiaRtia = _cfg->param.LpTiaRtia,
    };

    _utility_HSTIACfg_Type = (AD5940_ELECTROCHEMICAL_HSTIACfg_Type) {
        .DiodeClose = bFALSE,
        .ExtRtia = bFALSE,
        .HstiaCtia = 31,
        .HstiaDeRload = HSTIADERLOAD_OPEN,
        .HstiaDeRtia = HSTIADERTIA_OPEN,
        .HstiaRtiaSel = _cfg->param.HstiaRtiaSel,
    };

    _utility_DSPCfg_Type = (AD5940_ELECTROCHEMICAL_DSPCfg_Type) {
        .ADCDigCompCfg = {0},
        .ADCFilterCfg = {
            .ADCAvgNum = _cfg->param.ADCAvgNum,
            .ADCRate = _cfg->param.ADCRate,
            .ADCSinc2Osr = _cfg->param.ADCSinc2Osr,
            .ADCSinc3Osr = _cfg->param.ADCSinc3Osr,
            .BpNotch = _cfg->param.BpNotch,
            .BpSinc3 = _cfg->param.BpSinc3,
            // .DFTClkEnable
            // .Sinc2NotchClkEnable,
            .Sinc2NotchEnable = _cfg->param.Sinc2NotchEnable,
            // .Sinc3ClkEnable
            // .WGClkEnable
        },
        .ADCPga = _cfg->param.ADCPga,
        .DftCfg = {
            .DftNum = _cfg->param.DftNum,
            .DftSrc = _cfg->param.DftSrc,
            .HanWinEn = _cfg->param.HanWinEn,
        },
        .StatCfg = {0},
    };

    run_config = (AD5940_ELECTROCHEMICAL_RUN_CONFIG) {
        .agpio_cfg = &_cfg->param.agpio_cfg,
        .clock_cfg = &_cfg->param.clockConfig,
        .LFOSC_frequency = _cfg->param.lfoscFrequency,
        .DataType = _cfg->param.DataType,
        .FifoSrc = _cfg->param.FifoSrc,
        .FIFO_thresh = 1,   // Set to 1 to trigger callback whenever new ADC data is stored in the FIFO
    };

    lpdac_to_lptia_config = (AD5940_ELECTROCHEMICAL_LPDAC_TO_LPTIA_CONFIG) {
        .afe_ref_cfg = &_utility_AFERefCfg_Type,
        .dsp_cfg = &_utility_DSPCfg_Type,
        .lpdac_cfg = &_utility_LPPACfg_Type,
        .lptia_cfg = &_utility_LPTIACfg_Type,
    };

    /**
     * You need to set the following parameters:
     * - electrode_routing
     */
    lpdac_to_hstia_config = (AD5940_ELECTROCHEMICAL_LPDAC_TO_HSTIA_CONFIG) {
        .afe_ref_cfg = &_utility_AFERefCfg_Type,
        .dsp_cfg = &_utility_DSPCfg_Type,
        .lpdac_cfg = &_utility_LPPACfg_Type,
        .hstia_cfg = &_utility_HSTIACfg_Type,
        .electrode_routing = &_routing,
    };

    hsdac_mmr_to_hstia_config = (AD5940_ELECTROCHEMICAL_HSDAC_MMR_TO_HSTIA_CONFIG) {
        
    };

    atomic_store(&_state, AD5940_TASK_COMMAND_STATE_IDLE);

	for (;;) {
        err = AD5940_TASK_COMMAND_wait_measurement();
		if (err) {
            atomic_store(&_state, AD5940_TASK_COMMAND_STATE_ERROR);
            return err;
        }
        atomic_store(&_state, AD5940_TASK_COMMAND_STATE_EXECUTING);
        
        // callback
        if(_cfg->callback.start != NULL)
        {
            _cfg->callback.start();
        }

        AD5940_TASK_COMMAND_get_access_measurement_param_lock();

        // Measure the electrochemical.
        uint16_t adc_length;
        switch (_type)
        {
        case AD5940_TASK_ELECTROCHEMICAL_TYPE_CA: 
        {
            AD5940_ELECTROCHEMICAL_CA_CONFIG _config = {
                .parameters = &_parameters.ca.ad5940_parameters,
                .run = &run_config,
                .path_type = 1,
                .path.lpdac_to_hstia = &lpdac_to_hstia_config,
            };

            err = AD5940_ELECTROCHEMICAL_CA_start(
                &_config
            );
            if(err != AD5940ERR_OK) return err;

            // ADC length
            #define CA_FIFO_THRESH(t_interval, t_run) (round(t_run / t_interval) + 1)
            adc_length = CA_FIFO_THRESH(
                _parameters.ca.ad5940_parameters.t_interval, 
                _parameters.ca.t_run
            );
            AD5940_TASK_ADC_reset_length(adc_length);
            break;
        }
        case AD5940_TASK_ELECTROCHEMICAL_TYPE_CV: 
        {
            AD5940_ELECTROCHEMICAL_CV_CONFIG _config = {
                .parameters = &_parameters.cv.ad5940_parameters,
                .run = &run_config,
                .path_type = 1,
                .path.lpdac_to_hstia = &lpdac_to_hstia_config,
            };

            err = AD5940_ELECTROCHEMICAL_CV_start(
                &_config
            );
            if(err != AD5940ERR_OK) return err;

            // ADC length
            err = AD5940_ELECTROCHEMICAL_CV_get_fifo_count(
                &_parameters.cv.ad5940_parameters,
                &adc_length
            );
            if(err != AD5940ERR_OK) break;
            adc_length *= _parameters.cv.number_of_scans;
            AD5940_TASK_ADC_reset_length(adc_length);
            break;
        }
        case AD5940_TASK_ELECTROCHEMICAL_TYPE_DPV: 
        {
            AD5940_ELECTROCHEMICAL_DPV_CONFIG _config = {
                .parameters = &_parameters.dpv.ad5940_parameters,
                .run = &run_config,
                .path_type = 1,
                .path.lpdac_to_hstia = &lpdac_to_hstia_config,
            };

            err = AD5940_ELECTROCHEMICAL_DPV_start(
                &_config
            );
            if(err != AD5940ERR_OK) return err;

            // ADC length
            err = AD5940_ELECTROCHEMICAL_DPV_get_fifo_count(
                &_parameters.dpv.ad5940_parameters,
                &adc_length
            );
            if(err != AD5940ERR_OK) break;
            AD5940_TASK_ADC_reset_length(adc_length);
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
