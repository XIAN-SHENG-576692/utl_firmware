#include "ad5940_application_impl_zephyr.h"

#include <zephyr/kernel.h>

// ==================================================
// ADC
static K_MUTEX_DEFINE(_mutex_adc_state);
static K_MUTEX_DEFINE(_mutex_adc_length);
#define MAX_QUEUE 8
K_MSGQ_DEFINE(_quene_adc, sizeof(AD5940_APPLICATION_ADC_RESULT), MAX_QUEUE, 4);

// ==================================================
// Command
static K_MUTEX_DEFINE(_mutex_command_state);
static K_MUTEX_DEFINE(_mutex_command_measurement_param);

static K_MUTEX_DEFINE(_mutex_measurement_triggered);
static K_CONDVAR_DEFINE(_condvar_measurement_triggered);

int ad5940_application_init_impl_zephyr(void)
{
    int err = 0;
    err = k_mutex_init(&_mutex_adc_state);
    if(err) return err;
    err = k_mutex_init(&_mutex_adc_length);
    if(err) return err;

    err = k_mutex_init(&_mutex_command_state);
    if(err) return err;
    err = k_mutex_init(&_mutex_command_measurement_param);
    if(err) return err;
    err = k_mutex_init(&_mutex_measurement_triggered);
    if(err) return err;
    err = k_condvar_init(&_condvar_measurement_triggered);
    if(err) return err;
    return 0;
}

// ==================================================
// ADC
int ad5940_application_adc_get_access_state_lock(void)
{
    k_mutex_lock(&_mutex_adc_state, K_FOREVER);
    return 0;
}

int ad5940_application_adc_release_access_state_lock(void)
{
    k_mutex_unlock(&_mutex_adc_state);
    return 0;
}

int ad5940_application_adc_get_access_length_lock(void)
{
    k_mutex_lock(&_mutex_adc_length, K_FOREVER);
    return 0;
}

int ad5940_application_adc_release_access_length_lock(void)
{
    k_mutex_unlock(&_mutex_adc_length);
    return 0;
}

int ad5940_application_adc_put_quene(AD5940_APPLICATION_ADC_RESULT adc_result)
{
    return k_msgq_put(&_quene_adc, &adc_result, K_NO_WAIT);
}

int ad5940_application_adc_take_quene(AD5940_APPLICATION_ADC_RESULT *const adc_result)
{
    return k_msgq_get(&_quene_adc, adc_result, K_FOREVER);
}

// ==================================================
// Command
int ad5940_application_command_get_access_state_lock(void)
{
    k_mutex_lock(&_mutex_command_state, K_FOREVER);
    return 0;
}

int ad5940_application_command_release_access_state_lock(void)
{
    k_mutex_unlock(&_mutex_command_state);
    return 0;
}

int ad5940_application_command_get_access_measurement_param_lock(void)
{
    k_mutex_lock(&_mutex_command_measurement_param, K_FOREVER);
    return 0;
}

int ad5940_application_command_release_access_measurement_param_lock(void)
{
    k_mutex_unlock(&_mutex_command_measurement_param);
    return 0;
}

int ad5940_application_command_wait_measurement(void)
{
    k_mutex_lock(&_mutex_measurement_triggered, K_FOREVER);
    k_condvar_wait(&_condvar_measurement_triggered, &_mutex_measurement_triggered, K_FOREVER);
    k_mutex_unlock(&_mutex_measurement_triggered);
    return 0;
}

int ad5940_application_command_trigger_measurement(void)
{
    k_mutex_lock(&_mutex_measurement_triggered, K_FOREVER);
    k_condvar_broadcast(&_condvar_measurement_triggered);
    k_mutex_unlock(&_mutex_measurement_triggered);
    return 0;
}
