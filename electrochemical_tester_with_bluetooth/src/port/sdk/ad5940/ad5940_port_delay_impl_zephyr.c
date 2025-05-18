#include "ad5940.h"

#include <zephyr/kernel.h>

void AD5940_Delay10us(uint32_t time)
{
    k_sleep(K_USEC(10 * time));
}
