#include "ModbusThingDecorator.h"

ModbusThingDecorator::ModbusThingDecorator(ModbusThingPtr component)
  : Thing(*component),
    _component(component) {
}
