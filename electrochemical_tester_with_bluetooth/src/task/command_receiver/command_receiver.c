#include "command_receiver.h"

#include "ad5940_application_command.h"

static const COMMAND_RECEIVER_CFG *_cfg;
static COMMAND_RECEIVER_STATE _state = COMMAND_RECEIVER_STATE_UNINITIALIZED;

#define BUFF_LENGTH 70
static uint8_t response[BUFF_LENGTH];

COMMAND_RECEIVER_STATE command_receiver_get_state(void)
{
    COMMAND_RECEIVER_STATE copy;
    _cfg->port.get_access_state_lock();
    copy = _state;
    _cfg->port.release_access_state_lock();
    return copy;
}

int command_receiver_run(const COMMAND_RECEIVER_CFG *const cfg)
{
    int err = 0;
    _cfg = cfg;

    uint8_t *command;
    uint16_t command_length;
    AD5940_APPLICATION_ELECTROCHEMICAL_TYPE electrochemical_type;
    ELECTROCHEMICAL_PARAMETERS_UNION parameters;
    AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING electrode_routing;

    _cfg->port.get_access_state_lock();
    _state = COMMAND_RECEIVER_STATE_IDLE;
    _cfg->port.release_access_state_lock();

	for (;;) {
        err = _cfg->port.wait_command_received(
            &command,
            &command_length
        );

		if (err) {
            _cfg->port.get_access_state_lock();
            _state = COMMAND_RECEIVER_STATE_ERROR;
            _cfg->port.release_access_state_lock();
            return err;
        }
        _cfg->port.get_access_state_lock();
        _state = COMMAND_RECEIVER_STATE_EXECUTING;
        _cfg->port.release_access_state_lock();

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
                AD5940_APPLICATION_COMMAND_STATE state = ad5940_application_command_get_state();
                if(state != AD5940_APPLICATION_COMMAND_STATE_IDLE) continue;
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
                sizeof(ELECTROCHEMICAL_PARAMETERS_UNION)
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

                memcpy(p1, &_cfg->param.ad5940_application_command_param->hsrtia_calibration_result, sizeof(_cfg->param.ad5940_application_command_param->hsrtia_calibration_result));
                p1 += sizeof(_cfg->param.ad5940_application_command_param->hsrtia_calibration_result);

                float adcPga;
                AD5940_map_ADCPGA(_cfg->param.ad5940_application_command_param->ADCPga, &adcPga);
                memcpy(p1, &adcPga, sizeof(adcPga));
                p1 += sizeof(adcPga);

                float adcRefVolt = _cfg->param.ad5940_application_command_param->ADCRefVolt;
                memcpy(p1, &adcRefVolt, sizeof(adcRefVolt));
                p1 += sizeof(adcRefVolt);

                // Calculate the packet length
                uint16_t p_len = p1 - p0;

                // Send packet
                _cfg->port.send_response(p0, p_len);
            }
            {
                ad5940_application_command_measure(
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
        _cfg->port.get_access_state_lock();
        _state = COMMAND_RECEIVER_STATE_IDLE;
        _cfg->port.release_access_state_lock();
	}

    return 0;
}
