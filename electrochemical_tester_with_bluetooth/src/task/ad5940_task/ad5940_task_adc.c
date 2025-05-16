#include "ad5940_task_adc.h"

#include <stdatomic.h>

#include "ad5940_task_private.h"

#include "AD5940_irq_handler.h"

static const AD5940_TASK_ADC_CFG *_cfg;
static volatile _Atomic AD5940_TASK_ADC_STATE _state = AD5940_TASK_ADC_STATE_UNINITIALIZED;

AD5940_TASK_ADC_STATE AD5940_TASK_ADC_get_state(void)
{
    return atomic_load(&_state);
}

static AD5940_TASK_ADC_RESULT _result = {};

int AD5940_TASK_ADC_reset(
    uint8_t flag,
    uint16_t length
)
{
    AD5940_TASK_ADC_get_access_length_lock();
    _result.flag = flag;
    _result.adc_data_index = 0;
    _result.adc_data_length = length;
    AD5940_TASK_ADC_release_access_length_lock();
    return 0;
}

int AD5940_TASK_ADC_take_result_quene(
    AD5940_TASK_ADC_RESULT *const result
)
{
    return AD5940_TASK_ADC_take_quene(result);
}

AD5940Err AD5940_TASK_ADC_run(AD5940_TASK_ADC_CFG *const cfg)
{
    int err = 0;
    _cfg = cfg;

    atomic_store(&_state, AD5940_TASK_ADC_STATE_IDLE);

	for (;;) {
        err = AD5940_TASK_ADC_wait_ad5940_intc_triggered();
		if (err) {
            atomic_store(&_state, AD5940_TASK_ADC_STATE_ERROR);
            return err;
        }
        atomic_store(&_state, AD5940_TASK_ADC_STATE_EXECUTING);

        // callback
        if(_cfg->callback.start != NULL)
        {
            _cfg->callback.start();
        }

        AD5940_TASK_ADC_get_access_length_lock();
        if(_result.adc_data_index < _result.adc_data_length)
        {
            err = AD5940_irq_handler(
                ((_result.adc_data_index + 1) < _result.adc_data_length) ? -1 : 0,
                FIFO_BUFFER_SIZE,
                _result.fifo_buffer, 
                &_result.fifo_count
            );
            if (err) {
                atomic_store(&_state, AD5940_TASK_ADC_STATE_ERROR);
                for(;;) {}
            }
            AD5940_TASK_ADC_put_quene(&_result);
            _result.adc_data_index++;
        }
        else
        {
            _result.adc_data_index = 0;
            _result.adc_data_length = 0;
        }
        AD5940_TASK_ADC_release_access_length_lock();

        atomic_store(&_state, AD5940_TASK_ADC_STATE_IDLE);

        // callback
        if(_cfg->callback.end != NULL)
        {
            _cfg->callback.end();
        }
	}

    return err;
}
