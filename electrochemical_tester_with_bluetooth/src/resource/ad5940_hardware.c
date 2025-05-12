#include "ad5940_hardware.h"

// Our circuit use GPIO7 as interrupt pin.
AGPIOCfg_Type AD5940_EXTERNAL_agpio_cfg = {
    .FuncSet = GP7_INT,
    .InputEnSet = 0,
    .OutputEnSet = AGPIO_Pin7,
    .OutVal = 0,
    .PullEnSet = AGPIO_Pin7,
};
