#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(virus_detector)

set(UTILS_DIR ../utils)
add_subdirectory(${UTILS_DIR}/ble ble)
target_link_libraries(app PRIVATE ble_simple)

add_definitions(-DCHIPSEL_594X)
add_subdirectory(${UTILS_DIR}/ic/ad5940 ad5940)
# target_link_libraries(app PRIVATE ad5940)

add_subdirectory(${UTILS_DIR}/adapter/electrochemical_to_ad5940 electrochemical_to_ad5940)

add_subdirectory(${UTILS_DIR}/kernel_sleep kernel_sleep)
target_link_libraries(app PRIVATE kernel_sleep)

add_subdirectory(${UTILS_DIR}/spi spi)
target_link_libraries(app PRIVATE spi)

# NORDIC SDK APP START
target_sources(app PRIVATE
  ./src/main.c
  ./src/device/ad5940/port/ad5940_port_delay.c
  ./src/device/ad5940/port/ad5940_port_intc0_impl_zephyr.c
  ./src/device/ad5940/port/ad5940_port_intc1_impl_zephyr.c
  ./src/device/ad5940/port/ad5940_port_reset_impl_zephyr.c
  ./src/device/ad5940/port/ad5940_port_spi_impl_zephyr.c
  ./src/device/ad5940/port/ad5940_port_useless.c
  ./src/device/ad5940/ad5940_hardware.c
  ./src/device/ad5940/ad5940_init.c
  ./src/device/ad5940/ad5940_lock.c
  ./src/device/fan/fan.c
  ./src/device/pump/pump.c
  ./src/device/solenoid_valve/solenoid_valve.c
)

# Include UART ASYNC API adapter if configured
target_sources_ifdef(CONFIG_BT_NUS_UART_ASYNC_ADAPTER app PRIVATE
  ./src/driver/uart_async_adapter.c
)

target_include_directories(app PRIVATE
  ./src/application
  ./src/device
  ./src/driver
  ./src/device/ad5940
  ./src/device/ad5940/port
  ./src/device/fan
  ./src/device/pump
  ./src/device/solenoid_valve
)
# NORDIC SDK APP END
