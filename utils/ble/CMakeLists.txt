add_library(ble INTERFACE
)

target_link_libraries(ble INTERFACE 
  ble_simple
)

add_subdirectory(
  ./ble_simple
)
