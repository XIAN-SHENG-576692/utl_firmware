#include "set_afe.h"

#include "ad5940.h"
#include "parameters.h"

void set_lp_loop(void)
{
    LPLoopCfg_Type lp_cfg;

    AD5940_StructInit(&lp_cfg, sizeof(lp_cfg));         /* Reset everything to zero(OFF) */
    /* Configure what we need below */
    lp_cfg.LpDacCfg.LpdacSel = LPDAC0;                  /* Select LPDAC0. Note LPDAC1 is available on ADuCM355 */
    lp_cfg.LpDacCfg.DacData12Bit = LPDACDAT_12_BIT_INIT;               /* Output midscale voltage (0.2V + 2.4V)/2 = 1.3V */
    lp_cfg.LpDacCfg.DacData6Bit = LPDACDAT_6_BIT_INIT;                    /* 6Bit DAC data */
    lp_cfg.LpDacCfg.DataRst =bFALSE;                    /* Do not keep DATA registers at reset status */
    lp_cfg.LpDacCfg.LpDacSW = 0 
        | LPDACSW_VBIAS2LPPA 
        | LPDACSW_VZERO2LPTIA
    ;
    // LPDACSW_VBIAS2LPPA|LPDACSW_VBIAS2PIN|LPDACSW_VZERO2LPTIA|LPDACSW_VZERO2PIN;
    lp_cfg.LpDacCfg.LpDacRef = LPDACREF_2P5;            /* Select internal 2.5V reference */
    lp_cfg.LpDacCfg.LpDacSrc = LPDACSRC_MMR;            /* The LPDAC data comes from MMR not WG in this case */
    lp_cfg.LpDacCfg.LpDacVbiasMux = LPDACVBIAS_12BIT;   /* Connect Vbias signal to 12Bit LPDAC output */
    lp_cfg.LpDacCfg.LpDacVzeroMux = LPDACVZERO_6BIT;    /* Connect Vzero signal to 6bit LPDAC output */
    lp_cfg.LpDacCfg.PowerEn = bTRUE;                    /* Power up LPDAC */
    lp_cfg.LpAmpCfg.LpAmpSel = LPAMP0;
    lp_cfg.LpAmpCfg.LpAmpPwrMod = LPAMPPWR_NORM;           /* Set low power amplifiers to normal power mode */
    lp_cfg.LpAmpCfg.LpPaPwrEn = bTRUE;                  /* Enable LP PA(potentialstat amplifier) power */
    lp_cfg.LpAmpCfg.LpTiaPwrEn = bTRUE;                /* Leave LPTIA power off */
    lp_cfg.LpAmpCfg.LpTiaRf = LPTIARF_SHORT;
    lp_cfg.LpAmpCfg.LpTiaRtia = LPTIARTIA_10K;
    lp_cfg.LpAmpCfg.LpTiaRload = LPTIARLOAD_100R;

    if(lp_cfg.LpAmpCfg.LpTiaRtia == LPTIARTIA_OPEN)
    {
        lp_cfg.LpAmpCfg.LpTiaSW = 0
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
        lp_cfg.LpAmpCfg.LpTiaSW = 0
            | LPTIASW(2)
            | LPTIASW(4)
            | LPTIASW(5)
            // | LPTIASW(12)
            // | LPTIASW(13)
        ;
        // LPTIASW(12)|LPTIASW(13)|LPTIASW(2)|LPTIASW(10)\
        |LPTIASW(5)|LPTIASW(9); /* Close these switches to make sure LP PA amplifier is closed loop */
    }

    AD5940_LPLoopCfgS(&lp_cfg);
}
