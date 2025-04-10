# add_library(ad5940 STATIC
#   ./application/electrochemical/ca/ad5940_electrochemical_ca_function.c
#   ./application/electrochemical/ca/ad5940_electrochemical_ca_struct.c
#   ./application/electrochemical/cv/ad5940_electrochemical_cv_function.c
#   ./application/electrochemical/cv/ad5940_electrochemical_cv_struct.c
#   ./application/electrochemical/dpv/ad5940_electrochemical_dpv_function.c
#   ./application/electrochemical/dpv/ad5940_electrochemical_dpv_struct.c
#   ./application/electrochemical/utils/ad5940_electrochemical_utils_afe_dac.c
#   ./application/electrochemical/utils/ad5940_electrochemical_utils_sop.c
#   ./application/electrochemical/utils/ad5940_electrochemical_utils_tia_adc.c
#   ./application/electrochemical/utils/ad5940_electrochemical_utils_working_electrode.c
#   ./application/temperature/utils/ad5940_temperature_function.c
#   ./library/ad5940.c
#   ./main/ad5940_main.c
#   ./utils/ad5940_utils_adc.c
#   ./utils/ad5940_utils_afe.c
#   ./utils/ad5940_utils_gpio.c
#   ./utils/ad5940_utils_hsdac.c
#   ./utils/ad5940_utils_lpdac.c
#   ./utils/ad5940_utils_power.c
#   ./utils/ad5940_utils_sequence_generator.c
# )

# target_include_directories(ad5940 PUBLIC
#   .
#   ./application
#   ./library
#   ./main
#   ./utils
#   ./application/electrochemical
#   ./application/temperature
#   ./application/electrochemical/utils
#   ./application/electrochemical/ca
#   ./application/electrochemical/cv
#   ./application/electrochemical/dpv
#   ./application/temperature/utils
# )

zephyr_library_sources(
  ./application/electrochemical/ca/ad5940_electrochemical_ca_function.c
  ./application/electrochemical/ca/ad5940_electrochemical_ca_struct.c
  ./application/electrochemical/cv/ad5940_electrochemical_cv_function.c
  ./application/electrochemical/cv/ad5940_electrochemical_cv_struct.c
  ./application/electrochemical/dpv/ad5940_electrochemical_dpv_function.c
  ./application/electrochemical/dpv/ad5940_electrochemical_dpv_struct.c
  ./application/electrochemical/eis/ad5940_electrochemical_eis_struct.c
  ./application/electrochemical/utils/afe_dac/ad5940_electrochemical_utils_afe_dac.c
  ./application/electrochemical/utils/afe_dac/ad5940_electrochemical_utils_potential.c
  ./application/electrochemical/utils/tia_adc/ad5940_electrochemical_utils_tia_adc.c
  ./application/electrochemical/utils/ad5940_electrochemical_utils_sop.c
  ./application/temperature/utils/ad5940_temperature_function.c
  ./library/ad5940.c
  ./main/ad5940_main.c
  ./utils/ad5940_utils_adc.c
  ./utils/ad5940_utils_afe.c
  ./utils/ad5940_utils_fifo.c
  ./utils/ad5940_utils_gpio.c
  ./utils/ad5940_utils_hsdac.c
  ./utils/ad5940_utils_lpdac.c
  ./utils/ad5940_utils_power.c
  ./utils/ad5940_utils_sequence_generator.c
)

zephyr_library_include_directories(
  .
  ./application
  ./library
  ./main
  ./utils
  ./application/electrochemical
  ./application/temperature
  ./application/electrochemical/utils
  ./application/electrochemical/utils/afe_dac
  ./application/electrochemical/utils/tia_adc
  ./application/electrochemical/ca
  ./application/electrochemical/cv
  ./application/electrochemical/dpv
  ./application/electrochemical/eis
  ./application/temperature/utils
)
