#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

void KERNEL_SLEEP_forever(void);
void KERNEL_SLEEP_yield(void);
void KERNEL_SLEEP_h(const uint64_t h);
void KERNEL_SLEEP_m(const uint64_t m);
void KERNEL_SLEEP_s(const uint64_t s);
void KERNEL_SLEEP_ms(const uint64_t ms);
void KERNEL_SLEEP_us(const uint64_t us);
void KERNEL_SLEEP_ns(const uint64_t ns);

#ifdef __cplusplus
}
#endif
