add_library(modbus STATIC
  ModbusDiscovery.cpp
  ModbusRequest.cpp
  ModbusResponse.cpp
  ModbusThing.cpp
)

target_link_libraries(modbus
PUBLIC
  things
)
