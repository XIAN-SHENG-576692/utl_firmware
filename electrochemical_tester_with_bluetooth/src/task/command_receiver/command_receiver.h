#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "ad5940_task_command.h"

// ==================================================
// PORT
typedef enum {
    COMMAND_RECEIVER_START_TYPE_STOP,
    COMMAND_RECEIVER_START_TYPE_ELECTROCHEMICAL_CA,
    COMMAND_RECEIVER_START_TYPE_ELECTROCHEMICAL_CV,
    COMMAND_RECEIVER_START_TYPE_ELECTROCHEMICAL_DPV,
} COMMAND_RECEIVER_START_TYPE;

typedef struct
{
    COMMAND_RECEIVER_START_TYPE type;
    union {
        struct {
        } stop;
        struct {
            AD5940_TASK_ELECTROCHEMICAL_PARAMETERS_UNION parameters;
            AD5940_ELECTROCHEMICAL_ELECTRODE_ROUTING routing;
        } electrochemical;
    } param;
} COMMAND_RECEIVER_START;

int COMMAND_RECEIVER_wait_command_received(
    COMMAND_RECEIVER_START *const start
);
// ==================================================

typedef struct
{
    void (*start)(void);
    void (*end)(void);
} COMMAND_RECEIVER_CALLBACK;

typedef struct
{
    AD5940_TASK_COMMAND_PARAM *ad5940_task_command_param;
} COMMAND_RECEIVER_PARAM;

typedef struct
{
    COMMAND_RECEIVER_CALLBACK callback;
    COMMAND_RECEIVER_PARAM param;
} COMMAND_RECEIVER_CFG;

int COMMAND_RECEIVER_run(
    const COMMAND_RECEIVER_CFG *const cfg
);

typedef enum {
    COMMAND_RECEIVER_STATE_UNINITIALIZED,
    COMMAND_RECEIVER_STATE_IDLE,
    COMMAND_RECEIVER_STATE_EXECUTING,
    COMMAND_RECEIVER_STATE_ERROR,
} COMMAND_RECEIVER_STATE;

COMMAND_RECEIVER_STATE COMMAND_RECEIVER_get_state(void);

#ifdef __cplusplus
}
#endif
