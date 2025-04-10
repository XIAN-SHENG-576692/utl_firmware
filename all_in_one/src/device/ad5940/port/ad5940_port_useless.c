#include "ad5940.h"

/* (Not used for now.) AD5940 has 8 GPIOs, some of them are connected to MCU. MCU can set or read the status of these pins. */
void AD5940_MCUGpioWrite(uint32_t data)
{
    return;
}
uint32_t AD5940_MCUGpioRead(uint32_t data)
{
    return 0;
}
void AD5940_MCUGpioCtrl(uint32_t data, BoolFlag flag)
{
    return;
}

/* Below functions are frequently used in example code but not necessary for library */
uint32_t AD5940_GetMCUIntFlag(void)
{
    return 0;
}
uint32_t AD5940_ClrMCUIntFlag(void)
{
    return 0;
}
uint32_t AD5940_MCUResourceInit(void *pCfg)
{
    return 0;
}
