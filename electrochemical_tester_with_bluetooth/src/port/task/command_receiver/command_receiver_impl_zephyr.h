#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

int command_receiver_init_impl_zephyr(void);

int command_receiver_get_access_state_lock(void);
int command_receiver_release_access_state_lock(void);

#ifdef __cplusplus
}
#endif
