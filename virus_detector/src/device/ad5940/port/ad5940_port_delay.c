#include "ad5940.h"

#include "kernel_sleep.h"

void AD5940_Delay10us(uint32_t time)
{
    KERNEL_SLEEP_us(10 * time);
}
