cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(utl_all_in_one)

set(UTILS_DIR ../utils)
add_subdirectory(${UTILS_DIR}/ble ble)
target_link_libraries(app PRIVATE ble_simple)

add_definitions(-DCHIPSEL_594X)
add_subdirectory(${UTILS_DIR}/ic/ad5940 ad5940)
# target_link_libraries(app PRIVATE ad5940)

add_subdirectory(${UTILS_DIR}/kernel_sleep kernel_sleep)
target_link_libraries(app PRIVATE kernel_sleep)

add_subdirectory(${UTILS_DIR}/gpio gpio)
target_link_libraries(app PRIVATE gpio)

add_subdirectory(${UTILS_DIR}/spi spi)
target_link_libraries(app PRIVATE spi)

# NORDIC SDK APP START
target_sources(app PRIVATE
  ./src/main.c
  ./src/application/test_ad5940_electrochemical_ca.c
  ./src/application/test_ad5940_electrochemical_calibration_result.c
  ./src/application/test_ad5940_electrochemical_calibration.c
  ./src/application/test_ad5940_electrochemical_cv.c
  ./src/application/test_ad5940_electrochemical_dpv.c
  ./src/application/test_ad5940_init.c
  ./src/application/test_ad5940_lock.c
  ./src/application/test_ad5940_parameters.c
  ./src/application/test_ad5940_temperature.c
  ./src/application/utl_ad5940_electrochemical_utility_parameters.c
  ./src/application/utl_ad5940_temperature_parameters.c
  ./src/port/ad5940/ad5940_port_delay_impl_zephyr.c
  ./src/port/ad5940/ad5940_port_gpio.c
  ./src/port/ad5940/ad5940_port_intc0_impl_zephyr.c
  ./src/port/ad5940/ad5940_port_intc1_impl_zephyr.c
  ./src/port/ad5940/ad5940_port_reset_impl_zephyr.c
  ./src/port/ad5940/ad5940_port_spi_impl_zephyr.c
  ./src/port/ad5940/ad5940_port_useless.c
  ./src/resources/ad5940_hardware.c
)

# Include UART ASYNC API adapter if configured
target_sources_ifdef(CONFIG_BT_NUS_UART_ASYNC_ADAPTER app PRIVATE
  ./src/driver/uart_async_adapter.c
)

target_include_directories(app PRIVATE
  ./src
  ./src/application
  ./src/driver
  ./src/port
  ./src/port/ad5940
  ./src/resources
  ./src/driver/ad5940
)
# NORDIC SDK APP END
