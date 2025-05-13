#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ad5940.h"
#include "ad5940_electrochemical_utils.h"

// ==================================================
// PORT
int AD5940_TASK_COMMAND_get_access_measurement_param_lock(void);
int AD5940_TASK_COMMAND_release_access_measurement_param_lock(void);

int AD5940_TASK_COMMAND_wait_measurement(void);
int AD5940_TASK_COMMAND_trigger_measurement(void);

// ==================================================
// Type

typedef enum {
    AD5940_TASK_ELECTROCHEMICAL_TYPE_CA,
    AD5940_TASK_ELECTROCHEMICAL_TYPE_CV,
    AD5940_TASK_ELECTROCHEMICAL_TYPE_DPV,
} AD5940_TASK_ELECTROCHEMICAL_TYPE;

// ==================================================
// Parameters

#include "ad5940_electrochemical_ca.h"
#include "ad5940_electrochemical_cv.h"
#include "ad5940_electrochemical_dpv.h"
#include "ad5940_electrochemical_eis.h"

typedef struct 
{
    AD5940_ELECTROCHEMICAL_CA_PARAMETERS ad5940_parameters;
    float t_run;
} 
AD5940_TASK_ELECTROCHEMICAL_CA;

typedef struct 
{
    AD5940_ELECTROCHEMICAL_CV_PARAMETERS ad5940_parameters;
    uint8_t number_of_scans;
} 
AD5940_TASK_ELECTROCHEMICAL_CV;

typedef struct 
{
    AD5940_ELECTROCHEMICAL_DPV_PARAMETERS ad5940_parameters;
} 
AD5940_TASK_ELECTROCHEMICAL_DPV;

typedef union {
    AD5940_TASK_ELECTROCHEMICAL_CA ca;
    AD5940_TASK_ELECTROCHEMICAL_CV cv;
    AD5940_TASK_ELECTROCHEMICAL_DPV dpv;
} AD5940_TASK_ELECTROCHEMICAL_PARAMETERS_UNION;

// ==================================================
// Config

typedef struct
{
    AGPIOCfg_Type agpio_cfg;

    fImpPol_Type lprtia_calibration_result;
    fImpPol_Type hsrtia_calibration_result;

    AD5940_ClockConfig clockConfig;
    float lfoscFrequency;

    // /**
    //  * Refer to pages 50 and 52 of the datasheet.
    //  * @ref HSTIARTIA_Const
    //  */
    uint32_t HstiaRtiaSel;
    
    uint32_t ADCSinc2Osr;
    uint32_t ADCSinc3Osr;

    float ADCRefVolt;

    uint32_t ADCAvgNum;
    uint32_t ADCRate;
    uint32_t ADCPga;
    
    uint32_t DftNum;
    uint32_t DftSrc;

    BoolFlag HanWinEn;

    uint32_t LpAmpPwrMod;

    BoolFlag BpNotch;
    BoolFlag BpSinc3;
    BoolFlag Sinc2NotchEnable;

    uint32_t LpTiaRf;
    uint32_t LpTiaRload;
    uint32_t LpTiaRtia;

    uint32_t DataType;
    uint32_t FifoSrc;
} AD5940_TASK_COMMAND_PARAM;

typedef struct
{
    int (*start)(void);
    int (*end)(void);
} AD5940_TASK_COMMAND_CALLBACK;

typedef struct
{
    AD5940_TASK_COMMAND_CALLBACK callback;
    AD5940_TASK_COMMAND_PARAM param;
} AD5940_TASK_COMMAND_CFG;

AD5940Err AD5940_TASK_COMMAND_run(AD5940_TASK_COMMAND_CFG *const cfg);

typedef enum {
    AD5940_TASK_COMMAND_STATE_UNINITIALIZED,
    AD5940_TASK_COMMAND_STATE_IDLE,
    AD5940_TASK_COMMAND_STATE_EXECUTING,
    AD5940_TASK_COMMAND_STATE_ERROR,
} AD5940_TASK_COMMAND_STATE;

AD5940_TASK_COMMAND_STATE AD5940_TASK_COMMAND_get_state(void);

int AD5940_TASK_COMMAND_measure(
    const AD5940_TASK_ELECTROCHEMICAL_TYPE type,
    const AD5940_TASK_ELECTROCHEMICAL_PARAMETERS_UNION *const parameters,
    const AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING *const routing
);

#ifdef __cplusplus
}
#endif
