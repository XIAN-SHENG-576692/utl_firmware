#include "bluetooth_config_lock.h"

#include <zephyr/kernel.h>

static K_MUTEX_DEFINE(_mutex);
static K_CONDVAR_DEFINE(_condvar);
static bool _config_done = false;

void bluetooth_config_lock_init(void)
{
    k_mutex_init(&_mutex);
    k_condvar_init(&_condvar);
    return;
}

void bluetooth_config_lock_set_ready(void)
{
    k_mutex_lock(&_mutex, K_FOREVER);
    _config_done = true;
    k_condvar_broadcast(&_condvar);
    k_mutex_unlock(&_mutex);
}

void bluetooth_config_lock_wait_ready(void)
{
    k_mutex_lock(&_mutex, K_FOREVER);
    while (!_config_done) {
        k_condvar_wait(&_condvar, &_mutex, K_FOREVER);
    }
    k_mutex_unlock(&_mutex);
}
