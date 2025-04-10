#include "utility_sleep.h"

#include <zephyr/kernel.h>

void utility_sleep(int64_t ms)
{
    k_sleep(K_MSEC(ms));
}
