#include "ad5940_struct.h"

AD5940Err AD5940_STRUCT_get_recommanded_AFERefCfg_Type(
    AFERefCfg_Type *const type, 
    const BoolFlag Hp1V8BuffEn, 
    const BoolFlag Lp1V1BuffEn, 
    const BoolFlag Hp1V8ThemBuff, 
    const BoolFlag LPDAC_enable
)
{
    // This option is for the DAC, but other functions are available to set it, so please ignore it.
    type->HpBandgapEn = bFALSE;

    // Set to recommanded options.
    type->Hp1V8ThemBuff = Hp1V8ThemBuff;
    type->Disc1V1Cap = bFALSE;
    type->Lp1V1BuffEn = Lp1V1BuffEn;
    type->Hp1V1BuffEn = bTRUE;
    type->Disc1V8Cap = bFALSE;
    type->Lp1V8BuffEn = bTRUE;
    type->Hp1V8Ilimit = bTRUE;
    type->Hp1V8BuffEn = Hp1V8BuffEn;
    
    // The option is useless
    type->LpRefBoostEn = bFALSE;

    /* LP reference control - turn off them to save power*/
    if(LPDAC_enable == bTRUE)    /* With bias voltage */
    {
        type->LpBandgapEn = bTRUE;
        type->LpRefBufEn = bTRUE;
    }
    else
    {
        type->LpBandgapEn = bFALSE;
        type->LpRefBufEn = bFALSE;
    }

    return AD5940ERR_OK;
}
