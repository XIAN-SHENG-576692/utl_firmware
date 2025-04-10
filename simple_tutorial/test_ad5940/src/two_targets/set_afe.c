#include "set_afe.h"

#include "ad5940.h"
#include "parameters.h"

void set_afe(void)
{
    AFERefCfg_Type aferef_cfg;

    /* Initialize everything to zero(false/OFF/PowerDown), only turn on what we need */
    AD5940_StructInit(&aferef_cfg, sizeof(aferef_cfg));

    aferef_cfg.HpBandgapEn = bTRUE;
    aferef_cfg.Hp1V1BuffEn = bTRUE;
    aferef_cfg.Hp1V8BuffEn = bTRUE;
    aferef_cfg.Disc1V1Cap = bFALSE;
    aferef_cfg.Disc1V8Cap = bFALSE;
    aferef_cfg.Hp1V8ThemBuff = bFALSE;
    aferef_cfg.Hp1V8Ilimit = bFALSE;
    aferef_cfg.Lp1V1BuffEn = bFALSE;
    aferef_cfg.Lp1V8BuffEn = bFALSE;
    /* LP reference control - turn off them to save power*/
    if(LpBiasVolt == bTRUE)    /* With bias voltage */
    {
        aferef_cfg.LpBandgapEn = bTRUE;
        aferef_cfg.LpRefBufEn = bTRUE;
    }
    else
    {
        aferef_cfg.LpBandgapEn = bFALSE;
        aferef_cfg.LpRefBufEn = bFALSE;
    }
    aferef_cfg.LpRefBoostEn = bFALSE;

    AD5940_REFCfgS(&aferef_cfg);                           /* Call reference configuration function */
}
