# ElsewhereEdge

ElsewhereEdge is a SunSpec - MQTT gateway. It collects measurement data from smart meters and solar inverters over Modbus TCP.

## Device support
Currently, the following SunSpec models are supported: 103, 160, 203. So, **ANY** solar inverter and smart meter that supports SunSpec / Modbus should also be supported here. As of now, the following devices are successfully tested:
### Solar inverters
- SMA STP10.0-3AV-40
- SMA STP8.0-3av-40
- SMA STP 15000TL-30
- SMA STP 20000TL-30
- SMA STP 25000TL-30

### Smart meters
- elgris SMART METER WiFi

## Requirements
- C++17
- Qt5 Core Network SerialBus (currently not working with Qt6)

## Installation (from source)
```
git clone https://github.com/mincequi/ElsewhereEdge
cd ElsewhereEdge
mkdir build
cd build
cmake ..
make
```
