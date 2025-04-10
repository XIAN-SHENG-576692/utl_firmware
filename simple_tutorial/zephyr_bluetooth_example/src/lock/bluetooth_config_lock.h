#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

void bluetooth_config_lock_init(void);
void bluetooth_config_lock_set_ready(void);
void bluetooth_config_lock_wait_ready(void);

#ifdef __cplusplus
}
#endif
