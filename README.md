# ElsewhereEdge

ElsewhereEdge is a SunSpec - MQTT gateway. It collects measurement data from smart meters and solar inverters over Modbus TCP and exports via MQTT.

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

## Auto detection
ElsewhereEdge will scan your subnet with a mask of /24. E.g. if an instance is running on a host with IPv4 address 192.168.12.34, all IP addresses from 192.168.12.1 - 192.168.12.254 will be port scanned on port 502. If a valid SunSpec header is returned, the host will be considered as a valid SunSpec device.
This incoporates a small limitation, that currently there is only **one** Modbus device per host address supported, even though Modbus TCP would support multiple SunSpec devices per IP address.

## MQTT API
After being connected, the sunspec models will be polled and published via MQTT every 3 seconds.

The MQTT topic follows this scheme: `/elsewhere_<mac address>/<unique sunspec id>/<model id>/`.
Under this topic there are two sub-topics: `live` and `stats`. The *statistics* show the min/max values of the current day, as well as some other collected information. Thus, the *stats* topic will not be updated as often as the *live* topic, which gets an update every 3 seconds.

The payload is formatted as JSON.

## Device support
Currently, the following SunSpec models are supported: 101, 103, 160, 203. So, **ANY** solar inverter and smart meter that supports SunSpec / Modbus should also be supported here. As of now, the following devices are successfully tested:
### Solar inverters
- SMA STP10.0-3AV-40
- SMA STP8.0-3av-40
- SMA STP 15000TL-30
- SMA STP 20000TL-30
- SMA STP 25000TL-30

### Smart meters
- elgris SMART METER WiFi
