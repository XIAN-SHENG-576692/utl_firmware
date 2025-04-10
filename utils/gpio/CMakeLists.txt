add_library(gpio INTERFACE)
target_include_directories(gpio INTERFACE
  .
)

if(ZEPHYR_BASE)
  zephyr_library_include_directories(
    .
    ./debounce
    ./debounce/zephyr
  )
  zephyr_library_sources(
    ./debounce/zephyr/gpio_debounce.c
  )
elseif(CONFIG_STM32)
else()
  message(FATAL_ERROR "Unsupported MCU configuration")
endif()
