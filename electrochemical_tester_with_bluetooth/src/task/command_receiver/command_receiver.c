#include "command_receiver.h"

#include <stdatomic.h>

#include "ad5940_task_command.h"

static const COMMAND_RECEIVER_CFG *_cfg;
static volatile _Atomic COMMAND_RECEIVER_STATE _state = COMMAND_RECEIVER_STATE_UNINITIALIZED;

#define BUFF_LENGTH 70
static uint8_t response[BUFF_LENGTH];

COMMAND_RECEIVER_STATE COMMAND_RECEIVER_get_state(void)
{
    return atomic_load(&_state);
}

int COMMAND_RECEIVER_run(const COMMAND_RECEIVER_CFG *const cfg)
{
    int err = 0;
    _cfg = cfg;

    uint8_t *command;
    uint16_t command_length;
    AD5940_TASK_ELECTROCHEMICAL_TYPE electrochemical_type;
    AD5940_TASK_ELECTROCHEMICAL_PARAMETERS_UNION parameters;
    AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING electrode_routing;

    atomic_store(&_state, COMMAND_RECEIVER_STATE_IDLE);

	for (;;) {
        err = COMMAND_RECEIVER_wait_command_received(
            &command,
            &command_length
        );

		if (err) {
            atomic_store(&_state, COMMAND_RECEIVER_STATE_ERROR);
            return err;
        }
        atomic_store(&_state, COMMAND_RECEIVER_STATE_EXECUTING);

        // callback
        if(_cfg->callback.start != NULL)
        {
            err = _cfg->callback.start();
        }
		if (err) {
            atomic_store(&_state, COMMAND_RECEIVER_STATE_ERROR);
            return err;
        }

        // Check
		if(command[0] != 0x01) 
		{
			continue;
		}

        switch (command[1])
        {
        case 0x00:
        {
            {
                AD5940_TASK_COMMAND_STATE state = AD5940_TASK_COMMAND_get_state();
                if(state != AD5940_TASK_COMMAND_STATE_IDLE) continue;
            }

            // Entity id
            uint32_t entity_id;
            memcpy(
                &entity_id,
                (command + (1 + 1)),
                sizeof(uint32_t)
            );

            // Get witch type of electrochemical we want to measure.
            electrochemical_type = command[(1 + 1 + sizeof(entity_id))];

            memcpy(
                &parameters,
                (command + (1 + 1 + sizeof(entity_id) + sizeof(electrochemical_type))),
                sizeof(AD5940_TASK_ELECTROCHEMICAL_PARAMETERS_UNION)
            );

            memcpy(
                &electrode_routing,
                (command + (1 + 1 + sizeof(entity_id) + sizeof(electrochemical_type) + sizeof(parameters))),
                sizeof(AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING)
            );

            {
                uint8_t *p0 = response;
                uint8_t *p1 = p0;

                *p1++ = 0x02;
                *p1++ = 0x01;

                memcpy(p1, &entity_id, sizeof(entity_id));
                p1 += sizeof(entity_id);

                memcpy(p1, &electrochemical_type, sizeof(electrochemical_type));
                p1 += sizeof(electrochemical_type);

                memcpy(p1, &parameters, sizeof(parameters));
                p1 += sizeof(parameters);

                memcpy(p1, &electrode_routing, sizeof(electrode_routing));
                p1 += sizeof(electrode_routing);

                memcpy(p1, &_cfg->param.ad5940_task_command_param->hsrtia_calibration_result, sizeof(_cfg->param.ad5940_task_command_param->hsrtia_calibration_result));
                p1 += sizeof(_cfg->param.ad5940_task_command_param->hsrtia_calibration_result);

                float adcPga;
                AD5940_map_ADCPGA(_cfg->param.ad5940_task_command_param->ADCPga, &adcPga);
                memcpy(p1, &adcPga, sizeof(adcPga));
                p1 += sizeof(adcPga);

                float adcRefVolt = _cfg->param.ad5940_task_command_param->ADCRefVolt;
                memcpy(p1, &adcRefVolt, sizeof(adcRefVolt));
                p1 += sizeof(adcRefVolt);

                // Calculate the packet length
                uint16_t p_len = p1 - p0;

                // Send packet
                COMMAND_RECEIVER_send_response(p0, p_len);
            }
            {
                AD5940_TASK_COMMAND_measure(
                    electrochemical_type,
                    &parameters,
                    &electrode_routing
                );
            }
            break;
        }
        case 0x01:
        {
            // TODO
            break;
        }
        default:
            break;
        }
        atomic_store(&_state, COMMAND_RECEIVER_STATE_IDLE);

        // callback
        if(_cfg->callback.end != NULL)
        {
            err = _cfg->callback.end();
        }
        if (err) {
            atomic_store(&_state, COMMAND_RECEIVER_STATE_ERROR);
            return err;
        }
	}

    return 0;
}
