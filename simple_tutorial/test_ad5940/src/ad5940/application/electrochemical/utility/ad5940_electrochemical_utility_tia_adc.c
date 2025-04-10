#include "ad5940_electrochemical_utility_tia_adc.h"

#include "ad5940_utility.h"
#include "ad5940_external_components.h"

static void _get_LPAmpCfg_Type_with_LPTIA(
    LPAmpCfg_Type *const type,
    const AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type *const utility_LPPACfg_Type,
    const AD5940_ELECTROCHEMICAL_UTILITY_LPTIACfg_Type *const utility_LPTIACfg_Type
)
{
    #if defined(CHIPSEL_M355)
    type->LpAmpSel = utility_LPPACfg_Type->LpAmpSel;
    #else
    type->LpAmpSel = LPAMP0;
    #endif
    type->LpAmpPwrMod = utility_LPPACfg_Type->LpAmpPwrMod;
    type->LpPaPwrEn = bTRUE;
    type->LpTiaPwrEn = bTRUE;
    type->LpTiaRf = utility_LPTIACfg_Type->LpTiaRf;
    type->LpTiaRload = utility_LPTIACfg_Type->LpTiaRload;
    type->LpTiaRtia = utility_LPTIACfg_Type->LpTiaRtia;
    if(type->LpTiaRtia == LPTIARTIA_OPEN)
    {
        type->LpTiaSW = 0
            | LPTIASW(2)
            | LPTIASW(4)
            | LPTIASW(5)
            | LPTIASW(9)
            // | LPTIASW(12)
            // | LPTIASW(13)
        ;
    }
    else
    {
        type->LpTiaSW = 0
            | LPTIASW(2)
            | LPTIASW(4)
            | LPTIASW(5)
            // | LPTIASW(12)
            // | LPTIASW(13)
        ;
    }
    return;
}

static void _get_LPAmpCfg_Type_with_HSTIA(
    LPAmpCfg_Type *const type,
    const AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type *const utility_LPPACfg_Type
)
{
    #if defined(CHIPSEL_M355)
    type->LpAmpSel = utility_LPPACfg_Type->LpAmpSel;
    #else
    type->LpAmpSel = LPAMP0;
    #endif
    type->LpAmpPwrMod = utility_LPPACfg_Type->LpAmpPwrMod;
    type->LpPaPwrEn = bTRUE;
    type->LpTiaPwrEn = bFALSE;
    type->LpTiaSW = 0
        | LPTIASW(0)
        | LPTIASW(2)
        | LPTIASW(4)
        | LPTIASW(7)
    ;
    return;
}










/**
 * @param LPDAC_enable  @see pages 30 and 45 of the datasheet. 
 *                      The excitation signal is DC+AC. This parameter decides the DC value in mV unit. 0.0mV means no DC bias. (Refer to page 30 and page 45)
 */
static void _get_HSTIACfg_Type(
    HSTIACfg_Type *const type,
    const AD5940_ELECTROCHEMICAL_UTILITY_HSTIACfg_Type *const utility_type,
    const BoolFlag LPDAC_enable
)
{
    type->DiodeClose = utility_type->DiodeClose;
    type->ExtRtia = utility_type->ExtRtia;
    type->HstiaCtia = utility_type->HstiaCtia;
    type->HstiaDeRload = utility_type->HstiaDeRload;
    type->HstiaDeRtia = utility_type->HstiaDeRtia;
    type->HstiaRtiaSel = utility_type->HstiaRtiaSel;
    type->ExtRtia = AD5940_EXTERNAL_COMPONENTS_get_HSRTIA();
    #if defined(CHIPSEL_M355)
    type->HstiaDe1Rtia = utility_type->HstiaDe1Rtia;
    type->HstiaDe1Rload = utility_type->HstiaDe1Rload;
    #endif

    // @see page 45 of the datasheet, With bias voltage */
    if(LPDAC_enable)
    {
        type->HstiaBias = HSTIABIAS_VZERO0;
    }
    else
    {
        type->HstiaBias = HSTIABIAS_1P1;
    }
    return;
}










typedef enum
{
    _TIA_SELECTION_NULL,
    _TIA_SELECTION_LPTIA,
    _TIA_SELECTION_HSTIA,
}
_TIA_SELECTION;

void _get_DSPCfg_Type(
    DSPCfg_Type *const type, 
    const _TIA_SELECTION tia_selection,
    const AD5940_ELECTROCHEMICAL_UTILITY_DSPCfg_Type *const utility_type
)
{
    switch (tia_selection)
    {
    case _TIA_SELECTION_LPTIA:
        type->ADCBaseCfg.ADCMuxN = ADCMUXN_LPTIA0_N;
        type->ADCBaseCfg.ADCMuxP = ADCMUXP_LPTIA0_P;
        break;
    case _TIA_SELECTION_HSTIA:
        type->ADCBaseCfg.ADCMuxN = ADCMUXN_HSTIA_N;
        type->ADCBaseCfg.ADCMuxP = ADCMUXP_HSTIA_P;
        break;
    default:
        break;
    }
    type->ADCBaseCfg.ADCPga = utility_type->ADCPga;

    memcpy(&(type->ADCDigCompCfg), &(utility_type->ADCDigCompCfg), sizeof(ADCDigComp_Type));
    memcpy(&(type->ADCFilterCfg), &(utility_type->ADCFilterCfg), sizeof(ADCFilterCfg_Type));
    memcpy(&(type->DftCfg), &(utility_type->DftCfg), sizeof(DFTCfg_Type));
    memcpy(&(type->StatCfg), &(utility_type->StatCfg), sizeof(StatCfg_Type));

    return;
}










AD5940Err AD5940_ELECTROCHEMICAL_UTILITY_LPDAC_LPTIA_ADC_config(
    const AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type *const utility_LPPACfg_Type,
    const AD5940_ELECTROCHEMICAL_UTILITY_LPTIACfg_Type *const utility_LPTIACfg_Type,
    const AD5940_ELECTROCHEMICAL_UTILITY_DSPCfg_Type *const utility_DSPCfg_Type
)
{
    LPAmpCfg_Type lp_amp_cfg;
    DSPCfg_Type dsp_cfg;

    _get_LPAmpCfg_Type_with_LPTIA(
        &lp_amp_cfg,
        utility_LPPACfg_Type,
        utility_LPTIACfg_Type
    );
    _get_DSPCfg_Type(
        &dsp_cfg, 
        _TIA_SELECTION_LPTIA,
        utility_DSPCfg_Type
    );

    AD5940_LPAMPCfgS(&lp_amp_cfg);
    AD5940_DSPCfgS(&dsp_cfg);

    return AD5940ERR_OK;
}










AD5940Err AD5940_ELECTROCHEMICAL_UTILITY_LPDAC_HSTIA_ADC_config(
    const AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE working_electrode,
    const AD5940_ELECTROCHEMICAL_UTILITY_LPPACfg_Type *const utility_LPPACfg_Type,
    const AD5940_ELECTROCHEMICAL_UTILITY_HSTIACfg_Type *const utility_HSTIACfg_Type,
    const AD5940_ELECTROCHEMICAL_UTILITY_DSPCfg_Type *const utility_DSPCfg_Type
)
{
    LPAmpCfg_Type lp_amp_cfg;
    HSTIACfg_Type hstia_cfg;
    SWMatrixCfg_Type sw_matrix_cfg;
    DSPCfg_Type dsp_cfg;

    _get_LPAmpCfg_Type_with_HSTIA(
        &lp_amp_cfg,
        utility_LPPACfg_Type
    );
    _get_HSTIACfg_Type(
        &hstia_cfg,
        utility_HSTIACfg_Type,
        bTRUE
    );
    AD5940_ELECTROCHEMICAL_UTILITY_get_switch_matrix_by_working_electrode(
        &sw_matrix_cfg, 
        working_electrode
    );
    _get_DSPCfg_Type(
        &dsp_cfg, 
        _TIA_SELECTION_HSTIA,
        utility_DSPCfg_Type
    );

    AD5940_LPAMPCfgS(&lp_amp_cfg);
    AD5940_HSTIACfgS(&hstia_cfg);
    AD5940_SWMatrixCfgS(&sw_matrix_cfg);
    AD5940_DSPCfgS(&dsp_cfg);

    return AD5940ERR_OK;
}
