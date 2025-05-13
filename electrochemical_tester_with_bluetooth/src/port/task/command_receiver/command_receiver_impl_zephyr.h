#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

int COMMAND_RECEIVER_init_impl_zephyr(void);

int COMMAND_RECEIVER_get_access_state_lock(void);
int COMMAND_RECEIVER_release_access_state_lock(void);

#ifdef __cplusplus
}
#endif
