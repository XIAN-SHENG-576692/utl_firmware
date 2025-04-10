add_library(kernel_sleep INTERFACE)
target_include_directories(kernel_sleep INTERFACE
  .
)

if(ZEPHYR_BASE)
  zephyr_library_include_directories(
    ./zephyr
  )
  zephyr_library_sources(
    ./zephyr/kernel_sleep_impl_zephyr.c
  )
elseif(CONFIG_STM32)
else()
  message(FATAL_ERROR "Unsupported MCU configuration")
endif()
