#include "ModbusThingDecorator.h"

ModbusThingDecorator::ModbusThingDecorator(std::unique_ptr<ModbusThing> component)
  : ModbusThing(*component),
    _component(std::move(component))
{}
