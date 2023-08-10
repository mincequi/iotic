add_library(modbus STATIC
  ModbusDiscovery.cpp
)

target_link_libraries(modbus
PUBLIC
  things
)
