#pragma once

#include <modbus/ModbusThing.h>

class ModbusThingDecorator : public ModbusThing {
public:
    ModbusThingDecorator(std::unique_ptr<ModbusThing> component);

    virtual void drink() = 0;

protected:
    void callComponentDrink() {
        if (_component) {
            //_component->drink();
        }
    }

private:
    std::unique_ptr<ModbusThing> _component;
};
