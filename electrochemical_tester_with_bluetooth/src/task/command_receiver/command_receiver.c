#include "command_receiver.h"

#include <stdatomic.h>

#include "ad5940_task_command.h"

#include "kernel_sleep.h"

#include "ad5940_utils.h"

static const COMMAND_RECEIVER_CFG *_cfg;
static volatile _Atomic COMMAND_RECEIVER_STATE _state = COMMAND_RECEIVER_STATE_UNINITIALIZED;

COMMAND_RECEIVER_STATE COMMAND_RECEIVER_get_state(void)
{
    return atomic_load(&_state);
}

static int _map_type_from_receiver_to_ad5940_task(
    const COMMAND_RECEIVER_START_TYPE origin,
    AD5940_TASK_TYPE *const new
)
{
    *new = (AD5940_TASK_TYPE) origin;
    return 0;
}

int COMMAND_RECEIVER_run(
    const COMMAND_RECEIVER_CFG *const cfg
)
{
    int err = 0;
    _cfg = cfg;
    
    COMMAND_RECEIVER_START start;

    atomic_store(&_state, COMMAND_RECEIVER_STATE_IDLE);

	for (;;) {
        err = COMMAND_RECEIVER_wait_command_received(
            &start
        );

		if (err) {
            atomic_store(&_state, COMMAND_RECEIVER_STATE_ERROR);
            for(;;) {}
        }
        atomic_store(&_state, COMMAND_RECEIVER_STATE_EXECUTING);

        // callback
        if(_cfg->callback.start != NULL)
        {
            _cfg->callback.start();
        }

        if(start.type == COMMAND_RECEIVER_START_TYPE_STOP)
        {
            AD5940_shutdown_afe_lploop_hsloop_dsp();
        }
        else
        {
            {
                AD5940_TASK_MEASUREMENT_PARAM param = {
                    .type = AD5940_TASK_TYPE_TEMPERATURE,
                    .param.temperature = {
                        .sampling_interval = 0.01,
                        .sampling_time = 0.01,
                        .TEMPSENS = 0,
                    },
                };
                AD5940_TASK_COMMAND_measure(&param);
            }
            KERNEL_SLEEP_ms(100);
            {
                AD5940_TASK_MEASUREMENT_PARAM param = {};
                _map_type_from_receiver_to_ad5940_task(
                    start.type,
                    &param.type
                );
                param.param.electrochemical.parameters = start.param.electrochemical.parameters;
                param.param.electrochemical.routing = start.param.electrochemical.routing;
                AD5940_TASK_COMMAND_measure(&param);
            }
        }

        atomic_store(&_state, COMMAND_RECEIVER_STATE_IDLE);

        // callback
        if(_cfg->callback.end != NULL)
        {
            _cfg->callback.end();
        }
	}

    return 0;
}
