#ifndef SUNSPECTYPES_H
#define SUNSPECTYPES_H

#include <bitset>
#include <cstdint>
#include <sstream>

namespace sunspec {

enum InverterOperatingState : uint16_t {
    off                              = 1,
    waitingForPvVoltage              = 2,
    starting                         = 3,
    mpp                              = 4,
    regulated                        = 5,
    shuttingDown                     = 6,
    error                            = 7,
    waitingForElectricUtilityCompany = 8
};
std::ostream& operator<<(std::ostream& os, sunspec::InverterOperatingState state);

enum InverterEvent : uint16_t {
    groundingError      = 0,
    dcOvervoltage       = 1,

    gridDisconnected    = 4,
    enclosureOpen       = 5,
    shutdownManually    = 6,
    overTemperature     = 7,
    overFrequency       = 8,
    underFrequency      = 9,
    acOverVoltage       = 10,
    acUnderVoltage      = 11,
    stringFuseDefective = 12,
    underTemperature    = 13,
    storageOrCommunicationError = 14,
    hardwareTestError   = 15
};
class InverterEvents : public std::bitset<16> {
    using bitset::bitset;
};
std::ostream& operator<<(std::ostream& os, const InverterEvents& value);

}

#endif // SUNSPECTYPES_H
