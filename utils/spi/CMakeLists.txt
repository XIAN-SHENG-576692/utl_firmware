add_library(spi INTERFACE)
target_include_directories(spi INTERFACE
  .
)

if(ZEPHYR_BASE)
  zephyr_library_include_directories(
    .
    ./zephyr
  )
  zephyr_library_sources(
    ./zephyr/spi.c
  )
elseif(CONFIG_STM32)
else()
  message(FATAL_ERROR "Unsupported MCU configuration")
endif()
