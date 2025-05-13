#include "ad5940_task_adc.h"

#include "ad5940_task_private.h"

#include "ad5940_electrochemical_interrupt.h"

static const AD5940_TASK_ADC_CFG *_cfg;
static AD5940_TASK_ADC_STATE _state = AD5940_TASK_ADC_STATE_UNINITIALIZED;

AD5940_TASK_ADC_STATE AD5940_TASK_ADC_get_state(void)
{
    AD5940_TASK_COMMAND_STATE copy;
    AD5940_TASK_ADC_get_access_state_lock();
    copy = _state;
    AD5940_TASK_ADC_release_access_state_lock();
    return copy;
}

static uint16_t _adc_buffer_index = 0;
static uint16_t _adc_buffer_length = 0;

int AD5940_TASK_ADC_reset_length(uint16_t length)
{
    AD5940_TASK_ADC_get_access_length_lock();
    _adc_buffer_index = 0;
    _adc_buffer_length = length;
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

    AD5940_TASK_ADC_get_access_state_lock();
    _state = AD5940_TASK_ADC_STATE_IDLE;
    AD5940_TASK_ADC_release_access_state_lock();

	for (;;) {
        err = AD5940_TASK_ADC_wait_ad5940_intc_triggered();
		if (err) {
            AD5940_TASK_ADC_get_access_state_lock();
            _state = AD5940_TASK_COMMAND_STATE_ERROR;
            AD5940_TASK_ADC_release_access_state_lock();
            return err;
        }
        AD5940_TASK_ADC_get_access_state_lock();
        _state = AD5940_TASK_COMMAND_STATE_EXECUTING;
        AD5940_TASK_ADC_release_access_state_lock();

        // callback
        if(_cfg->callback.start != NULL)
        {
            _cfg->callback.start();
        }

        AD5940_TASK_ADC_get_access_length_lock();
        AD5940_TASK_ADC_RESULT result = {
            .adc_buffer_index = _adc_buffer_index,
            .adc_buffer_length = _adc_buffer_length,
        };
        _adc_buffer_index++;
        uint16_t MCU_FIFO_count;
        err = AD5940_ELECTROCHEMICAL_interrupt(
            1,
            (_adc_buffer_index < _adc_buffer_length) ? -1 : 0,
            &result.adc_buffer, 
            &MCU_FIFO_count
        );
        if(_adc_buffer_index >= _adc_buffer_length)
        {
            _adc_buffer_index = 0;
            _adc_buffer_length = 0;
        }
        AD5940_TASK_ADC_put_quene(result);
        AD5940_TASK_ADC_release_access_length_lock();

        AD5940_TASK_ADC_get_access_state_lock();
        _state = AD5940_TASK_COMMAND_STATE_IDLE;
        AD5940_TASK_ADC_release_access_state_lock();

        // callback
        if(_cfg->callback.end != NULL)
        {
            _cfg->callback.end();
        }
	}

    return err;
}
