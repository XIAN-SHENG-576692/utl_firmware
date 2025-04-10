#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

void test_ad5940_lock_init(void);
void test_ad5940_lock_boardcast(void);
void test_ad5940_lock_wait(void);

#ifdef __cplusplus
}
#endif
