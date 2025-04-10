#include "test_ad5940_lock.h"

#include <zephyr/kernel.h>

static K_MUTEX_DEFINE(_mutex);
static K_CONDVAR_DEFINE(_condvar);

void test_ad5940_lock_init(void)
{
    k_mutex_init(&_mutex);
    k_condvar_init(&_condvar);
    return;
}

void test_ad5940_lock_boardcast(void)
{
    k_mutex_lock(&_mutex, K_FOREVER);
    k_condvar_broadcast(&_condvar);
    k_mutex_unlock(&_mutex);
    return;
}

void test_ad5940_lock_wait(void)
{
    k_mutex_lock(&_mutex, K_FOREVER);
    k_condvar_wait(&_condvar, &_mutex, K_FOREVER);
    k_mutex_unlock(&_mutex);
    return;
}
