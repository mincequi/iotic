#pragma once

class ConsumerInterface {
public:
    virtual int currentPower() const = 0;

    virtual void setAvailablePower(int watts) = 0;

private:
    ConsumerInterface();
};
