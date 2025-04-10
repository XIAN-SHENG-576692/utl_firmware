#include "ad5940_electrochemical_utility.h"

#include "ad5940.h"

AD5940Err AD5940_ELECTROCHEMICAL_UTILITY_get_switch_matrix_by_working_electrode(
    SWMatrixCfg_Type *const type,
    const AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE working_electrode
)
{
    type->Dswitch = SWD_CE0;
    type->Pswitch = SWP_RE0;
    switch (working_electrode)
    {
    case AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_SE0:
        type->Nswitch = SWN_SE0;
        type->Tswitch = SWT_SE0LOAD | SWT_TRTIA;
        break;
    case AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_DE0:
        type->Nswitch = SWN_DE0LOAD;
        type->Tswitch = SWT_DE0 | SWT_TRTIA;
        break;
    case AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_AIN0:
        type->Nswitch = SWN_AIN0;
        type->Tswitch = SWT_AIN0 | SWT_TRTIA;
        break;
    case AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_AIN1:
        type->Nswitch = SWN_AIN1;
        type->Tswitch = SWT_AIN1 | SWT_TRTIA;
        break;
    case AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_AIN2:
        type->Nswitch = SWN_AIN2;
        type->Tswitch = SWT_AIN2 | SWT_TRTIA;
        break;
    case AD5940_ELECTROCHEMICAL_WORKING_ELECTRODE_AIN3:
        type->Nswitch = SWN_AIN3;
        type->Tswitch = SWT_AIN3 | SWT_TRTIA;
        break;
    default:
        type->Dswitch = SWD_OPEN;
        type->Pswitch = SWP_OPEN;
        type->Nswitch = SWN_OPEN;
        type->Tswitch = SWT_OPEN;
        break;
    }
    return AD5940ERR_OK;
}
