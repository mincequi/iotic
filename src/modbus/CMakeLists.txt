add_library(modbus STATIC
  ModbusDiscovery.cpp
  ModbusThing.cpp
  ModbusThingDecorator.cpp
)

target_link_libraries(modbus
PUBLIC
  things
  http
)
