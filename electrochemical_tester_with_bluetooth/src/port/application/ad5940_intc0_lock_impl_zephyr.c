#include "ad5940_intc0_lock_impl_zephyr.h"
#include "ad5940_intc0_lock.h"

#include <zephyr/kernel.h>

static K_MUTEX_DEFINE(_mutex);
static K_CONDVAR_DEFINE(_condvar);

int ad5940_intc0_lock_init_impl_zephyr(void)
{
    int err = 0;
    err = k_mutex_init(&_mutex);
    if(err) return err;
    err = k_condvar_init(&_condvar);
    if(err) return err;
    return 0;
}

int ad5940_intc0_lock_boardcast_impl_zephyr(void)
{
    k_mutex_lock(&_mutex, K_FOREVER);
    k_condvar_broadcast(&_condvar);
    k_mutex_unlock(&_mutex);
    return 0;
}

int ad5940_intc0_lock_wait(void)
{
    k_mutex_lock(&_mutex, K_FOREVER);
    k_condvar_wait(&_condvar, &_mutex, K_FOREVER);
    k_mutex_unlock(&_mutex);
    return 0;
}
