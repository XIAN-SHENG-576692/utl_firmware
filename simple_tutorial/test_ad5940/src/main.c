#include "ble.h"

#include "electrochemical_ca.h"
#include "electrochemical_cv.h"
#include "electrochemical_dpv.h"

ELECTROCHEMICAL_CA_PARAMETERS ca;
ELECTROCHEMICAL_CV_PARAMETERS cv;
ELECTROCHEMICAL_DPV_PARAMETERS dpv;

#define BUFFER_SIZE 1000

int16_t ca_voltages[BUFFER_SIZE];
int16_t cv_voltages[BUFFER_SIZE];
int16_t dpv_voltages[BUFFER_SIZE];

uint16_t ca_voltages_length = 0;
uint16_t cv_voltages_length = 0;
uint16_t dpv_voltages_length = 0;

/* 主函式 */
int main(void)
{
    int error;

    error = BLE_init();
    if(error) return error;

    ca = (ELECTROCHEMICAL_CA_PARAMETERS) {
        .e_dc = 3.5e3,
        .t_interval = 1e3,
        .t_run = 15e3,
    };
    cv = (ELECTROCHEMICAL_CV_PARAMETERS) {
        .e_begin = 0.6e3,
        .e_step = 0.1e3,
        .e_vertex1 = 0.6e3,
        .e_vertex2 = -0.6e3,
        .number_of_scans = 3,
        .scan_rate = 0.05e3,
    };
    dpv = (ELECTROCHEMICAL_DPV_PARAMETERS) {
        .e_begin = -0.6e3,
        .e_end = 0.6e3,
        .e_step = 0.01e3,
        .e_pulse = 0.05e3,
        .t_pulse = 0.05e3,
        .scan_rate = 0.05e3,
        .inversion_option = ELECTROCHEMICAL_DPV_INVERSION_OPTION_INVERT_NONE,
    };

    ELECTROCHEMICAL_CA_get_voltages(
        &ca,
        BUFFER_SIZE,
        ca_voltages,
        &ca_voltages_length
    );

    ELECTROCHEMICAL_CV_get_voltages(
        &cv,
        BUFFER_SIZE,
        cv_voltages,
        &cv_voltages_length
    );
    
    ELECTROCHEMICAL_DPV_get_voltages(
        &dpv,
        ELECTROCHEMICAL_DPV_ARRAY_OPTION_BOTH,
        BUFFER_SIZE,
        dpv_voltages,
        &dpv_voltages_length
    );

	return 0;
}
