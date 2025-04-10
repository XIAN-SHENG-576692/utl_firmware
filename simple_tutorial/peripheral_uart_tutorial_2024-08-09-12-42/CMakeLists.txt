#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(utl_all_in_one)

# NORDIC SDK APP START
target_sources(app PRIVATE
  src/main.c
  src/device/device_ble.c
  src/device/device_buttons.c
  src/device/device_lights.c
  src/driver/driver_uart.c
  src/task/task_ble.c
  src/task/task_buttons.c
  src/task/task_thread.c
  src/utility/utility_sleep.c
)

# Include UART ASYNC API adapter
target_sources_ifdef(CONFIG_BT_NUS_UART_ASYNC_ADAPTER app PRIVATE
  src/driver/uart_async_adapter.c
)

# NORDIC SDK APP END

zephyr_library_include_directories(
  .
  ./src
  ./src/device
  ./src/driver
  ./src/task
  ./src/utility
)