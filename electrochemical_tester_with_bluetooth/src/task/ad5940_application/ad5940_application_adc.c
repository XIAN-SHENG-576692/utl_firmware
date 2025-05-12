#include "ad5940_application_adc.h"

#include "ad5940_application_private.h"

#include "ad5940_electrochemical_interrupt.h"

static const AD5940_APPLICATION_ADC_CFG *_cfg;
static AD5940_APPLICATION_ADC_STATE _state = AD5940_APPLICATION_ADC_STATE_UNINITIALIZED;

AD5940_APPLICATION_ADC_STATE ad5940_application_adc_get_state(void)
{
    AD5940_APPLICATION_COMMAND_STATE copy;
    _cfg->port.get_access_state_lock();
    copy = _state;
    _cfg->port.release_access_state_lock();
    return copy;
}

static uint16_t _adc_buffer_index = 0;
static uint16_t _adc_buffer_length = 0;

int ad5940_application_adc_reset_length(uint16_t length)
{
    _cfg->port.get_access_length_lock();
    _adc_buffer_index = 0;
    _adc_buffer_length = length;
    _cfg->port.release_access_length_lock();
    return 0;
}

int ad5940_application_adc_take_result_quene(
    AD5940_APPLICATION_ADC_RESULT *const result
)
{
    return _cfg->port.take_quene(result);
}

AD5940Err ad5940_application_adc_run(AD5940_APPLICATION_ADC_CFG *const cfg)
{
    int err = 0;
    _cfg = cfg;

    _cfg->port.get_access_state_lock();
    _state = AD5940_APPLICATION_ADC_STATE_IDLE;
    _cfg->port.release_access_state_lock();

	for (;;) {
        err = _cfg->port.wait_ad5940_intc_triggered();
		if (err) {
            _cfg->port.get_access_state_lock();
            _state = AD5940_APPLICATION_COMMAND_STATE_ERROR;
            _cfg->port.release_access_state_lock();
            return err;
        }
        _cfg->port.get_access_state_lock();
        _state = AD5940_APPLICATION_COMMAND_STATE_EXECUTING;
        _cfg->port.release_access_state_lock();

        _cfg->port.get_access_length_lock();
        AD5940_APPLICATION_ADC_RESULT result = {
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
        _cfg->port.put_quene(result);
        _cfg->port.release_access_length_lock();

        _cfg->port.get_access_state_lock();
        _state = AD5940_APPLICATION_COMMAND_STATE_IDLE;
        _cfg->port.release_access_state_lock();
	}

    return err;
}
