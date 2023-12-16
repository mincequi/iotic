#pragma once

#include <modbus/ModbusThing.h>

class ModbusThingDecorator : public Thing {
public:
    ModbusThingDecorator(ModbusThingPtr component);

protected:


private:
    ModbusThingPtr _component;
};
