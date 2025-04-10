#include "kernel_sleep_impl_zephyr.h"

#include <zephyr/kernel.h>

void KERNEL_SLEEP_forever(void)
{
    k_sleep(K_FOREVER);
}

void KERNEL_SLEEP_ns(const uint64_t ns)
{
    k_sleep(K_NSEC(ns));
}

void KERNEL_SLEEP_us(const uint64_t us)
{
    k_sleep(K_USEC(us));
}

void KERNEL_SLEEP_ms(const uint64_t ms)
{
    k_sleep(K_MSEC(ms));
}

void KERNEL_SLEEP_s(const uint64_t s)
{
    k_sleep(K_SECONDS(s));
}

void KERNEL_SLEEP_m(const uint64_t m)
{
    k_sleep(K_MINUTES(m));
}

void KERNEL_SLEEP_h(const uint64_t h)
{
    k_sleep(K_HOURS(h));
}

void KERNEL_SLEEP_yield(void)
{
    k_yield();
}
