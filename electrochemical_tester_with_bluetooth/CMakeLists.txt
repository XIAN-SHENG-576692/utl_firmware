cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(utl_all_in_one)

get_filename_component(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
set(UTILS_DIR ${PROJECT_DIR}/../utils)

add_subdirectory(${UTILS_DIR}/ble ble)
target_link_libraries(app PRIVATE ble_simple)

add_definitions(-DCHIPSEL_594X)
set(AD5940_DIR ${UTILS_DIR}/ic/ad5940)
include(${AD5940_DIR}/cmake/ad5940.cmake)
import_ad5940(app ${AD5940_DIR})

add_subdirectory(${UTILS_DIR}/kernel_sleep kernel_sleep)
target_link_libraries(app PRIVATE kernel_sleep)

add_subdirectory(${UTILS_DIR}/gpio gpio)
target_link_libraries(app PRIVATE gpio)

add_subdirectory(${UTILS_DIR}/spi spi)
target_link_libraries(app PRIVATE spi)

# NORDIC SDK APP START
target_sources(app PRIVATE
  ./src/main.c
  ./src/application/ad5940_clock_init.c
  ./src/application/ad5940_electrochemical_calibration.c
  ./src/port/application/ad5940_intc0_lock_impl_zephyr.c
  ./src/port/sdk/ad5940/ad5940_port_delay_impl_zephyr.c
  ./src/port/sdk/ad5940/ad5940_port_gpio_impl_zephyr.c
  ./src/port/sdk/ad5940/ad5940_port_intc0_impl_zephyr.c
  ./src/port/sdk/ad5940/ad5940_port_intc1_impl_zephyr.c
  ./src/port/sdk/ad5940/ad5940_port_reset_impl_zephyr.c
  ./src/port/sdk/ad5940/ad5940_port_spi_impl_zephyr.c
  ./src/port/sdk/ad5940/ad5940_port_useless.c
  ./src/port/task/ad5940_task/ad5940_task_impl_zephyr.c
  ./src/port/task/command_receiver/command_receiver_impl_zephyr.c
  ./src/resource/ad5940_hardware.c
  ./src/wdt/watchdog0_wdt.c
  ./src/task/ad5940_task/ad5940_task_adc.c
  ./src/task/ad5940_task/ad5940_task_command.c
  ./src/task/command_receiver/command_receiver.c
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
  ./src/port/sdk
  ./src/resource
  ./src/wdt
  ./src/task
  ./src/driver/ad5940
  ./src/port/application
  ./src/task/ad5940_task
  ./src/task/command_receiver
  ./src/port/sdk/ad5940
  ./src/port/task/ad5940_task
  ./src/port/task/command_receiver
)
# NORDIC SDK APP END
