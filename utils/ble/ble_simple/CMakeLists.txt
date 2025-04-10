add_library(ble_simple INTERFACE
)

target_include_directories(ble_simple INTERFACE 
  .
)

if(ZEPHYR_BASE)
  zephyr_library_include_directories(
    ./zephyr
  )
  zephyr_library_sources(
    ./zephyr/ble_simple_impl_zephyr.c
  )
elseif(CONFIG_STM32)
else()
  message(FATAL_ERROR "Unsupported MCU configuration")
endif()
