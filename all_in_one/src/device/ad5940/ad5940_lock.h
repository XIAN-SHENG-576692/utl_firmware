#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

void ad5940_afeintc0_lock_init(void);
void ad5940_afeintc0_lock_boardcast(void);
void ad5940_afeintc0_lock_wait(void);

#ifdef __cplusplus
}
#endif
