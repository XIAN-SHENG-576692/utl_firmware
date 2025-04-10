cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zephyr_bluetooth_example)

target_sources(app PRIVATE 
    src/main.c
    src/config/bluetooth_config.c
    src/lock/bluetooth_config_lock.c
    src/on_receive/on_receive_back.c
    ./src/thread/bluetooth_send_random_bytes_thread.c
)

zephyr_library_include_directories(
    .
    ./src
    ./src/config
    ./src/lock
    ./src/on_receive
    ./src/thread
)
