#include "command_receiver_impl_zephyr.h"

#include <zephyr/kernel.h>

static K_MUTEX_DEFINE(_mutex_state);

int COMMAND_RECEIVER_init_impl_zephyr(void)
{
    int err = 0;

    err = k_mutex_init(&_mutex_state);
    if(err) return err;

    return 0;
}

int COMMAND_RECEIVER_get_access_state_lock(void)
{
    k_mutex_lock(&_mutex_state, K_FOREVER);
    return 0;
}

int COMMAND_RECEIVER_release_access_state_lock(void)
{
    k_mutex_unlock(&_mutex_state);
    return 0;
}
