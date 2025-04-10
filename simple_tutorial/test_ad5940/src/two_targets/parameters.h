#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#define LpBiasVolt bTRUE

#define LPDACDAT_12_BIT_INIT 0x800
#define LPDACDAT_6_BIT_INIT 0x20

#define HSDACDAT_BIT_INIT 0x800

#define EXCITBUFGAIN EXCITBUFGAIN_2
#define HSDACGAIN HSDACGAIN_1

#define HsBiasVolt bFALSE

#define BUFF_SIZE 1000
extern uint32_t seq_gen_buffer[BUFF_SIZE];

#define ELECTROCHEMICAL_UNIT 1e3

#include "electrochemical_dpv.h"

extern ELECTROCHEMICAL_DPV_PARAMETERS dpv_lp_loop;

extern ELECTROCHEMICAL_DPV_PARAMETERS dpv_hs_loop;

#define ADC_SEQ_ID SEQID_0
#define DPV_STEP_SEQ_ID SEQID_1
#define DPV_PULSE_SEQ_ID SEQID_2

#ifdef __cplusplus
}
#endif
