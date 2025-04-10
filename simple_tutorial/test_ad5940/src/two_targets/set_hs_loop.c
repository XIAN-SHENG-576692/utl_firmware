#include "set_afe.h"

#include "ad5940.h"
#include "parameters.h"

void set_hs_loop(void)
{
    AD5940_AFECtrlS(AFECTRL_HSTIAPWR, bTRUE);
    if(HsBiasVolt == bTRUE)
    AD5940_AFECtrlS(AFECTRL_HSTIAPWR|AFECTRL_INAMPPWR|AFECTRL_EXTBUFPWR|\
                AFECTRL_WG|AFECTRL_DACREFPWR|AFECTRL_HSDACPWR|\
                AFECTRL_SINC2NOTCH, bTRUE);
    else
    AD5940_AFECtrlS(AFECTRL_HSTIAPWR|AFECTRL_INAMPPWR|AFECTRL_EXTBUFPWR|\
                AFECTRL_WG|AFECTRL_DACREFPWR|AFECTRL_HSDACPWR|\
                AFECTRL_SINC2NOTCH|AFECTRL_DCBUFPWR, bTRUE);

    HSLoopCfg_Type hs_cfg;

    AD5940_StructInit(&hs_cfg, sizeof(hs_cfg));         /* Reset everything to zero(OFF) */

    hs_cfg.HsDacCfg.ExcitBufGain = EXCITBUFGAIN;
    hs_cfg.HsDacCfg.HsDacGain = HSDACGAIN;
    hs_cfg.HsDacCfg.HsDacUpdateRate = 255;

    hs_cfg.HsTiaCfg.DiodeClose = bFALSE;

    if(HsBiasVolt == bTRUE)    /* With bias voltage */
    {
        hs_cfg.HsTiaCfg.HstiaBias = HSTIABIAS_VZERO0;
    }
    else
    {
        hs_cfg.HsTiaCfg.HstiaBias = HSTIABIAS_1P1;
    }

    hs_cfg.HsTiaCfg.HstiaCtia = 31; /* 31pF + 2pF */
    hs_cfg.HsTiaCfg.HstiaDeRload = HSTIADERLOAD_OPEN;
    hs_cfg.HsTiaCfg.HstiaDeRtia = HSTIADERTIA_OPEN;
    hs_cfg.HsTiaCfg.HstiaRtiaSel = HSTIARTIA_10K;

    hs_cfg.HsTiaCfg.ExtRtia = 1e4;

    hs_cfg.SWMatCfg.Dswitch = SWD_AIN1;
    hs_cfg.SWMatCfg.Pswitch = SWP_AIN2;
    hs_cfg.SWMatCfg.Nswitch = SWN_AIN3;
    hs_cfg.SWMatCfg.Tswitch = SWT_AIN3 | SWT_TRTIA;

    hs_cfg.WgCfg.WgType = WGTYPE_MMR;
    hs_cfg.WgCfg.GainCalEn = bTRUE;
    hs_cfg.WgCfg.OffsetCalEn = bTRUE;
    hs_cfg.WgCfg.WgCode = HSDACDAT_BIT_INIT;

    AD5940_HSLoopCfgS(&hs_cfg);
}
