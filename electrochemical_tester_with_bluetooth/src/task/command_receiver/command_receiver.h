#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "ad5940_application_command.h"

typedef struct
{
    int (*wait_command_received)(
        uint8_t **const command,
        uint16_t *const command_length
    );
    int (*send_response)(
        uint8_t *const command,
        const uint16_t command_length
    );
    int (*get_access_state_lock)(void);
    int (*release_access_state_lock)(void);
} COMMAND_RECEIVER_PORT;

typedef struct
{
    AD5940_APPLICATION_COMMAND_PARAM *ad5940_application_command_param;
} COMMAND_RECEIVER_PARAM;

typedef struct
{
    COMMAND_RECEIVER_PORT port;
    COMMAND_RECEIVER_PARAM param;
} COMMAND_RECEIVER_CFG;

int command_receiver_run(const COMMAND_RECEIVER_CFG *const cfg);

typedef enum {
    COMMAND_RECEIVER_STATE_UNINITIALIZED,
    COMMAND_RECEIVER_STATE_IDLE,
    COMMAND_RECEIVER_STATE_EXECUTING,
    COMMAND_RECEIVER_STATE_ERROR,
} COMMAND_RECEIVER_STATE;

COMMAND_RECEIVER_STATE command_receiver_get_state(void);

#ifdef __cplusplus
}
#endif
