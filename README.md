# Iotic

Iotic is a multi-purpose IoT device manager which focuses on thermal and electrical energy.
Photovoltaic systems, electric vehicles and heatings can be brought together for smart energy management.
It also can act as a data collector and export to InfluxDB and MQTT. This allows multiple scenarios (e.g.):
- SunSpec - MQTT gateway
- PV surplus controlled charging of electric cars
- PV surplus water controlled water heating
- Peak shaving

Its main features are:
- Modbus support
- SunSpec device support (Solar inverters, smart meters)
- Shelly support (1 and 1PM)
- go-e Charger support
- MQTT export
- InfluxDB export
- Web app

## Requirements
- C++17
- Qt (>=6.4) Core Network SerialBus WebSockets HttpServer (currently not working with Qt5)
- Flutter

## Installation (from source)
### Prerequisites (Debian/Ubuntu)
```
sudo apt install \
  build-essential \
  cmake \
  libcurl4-openssl-dev \
  libqt6serialbus6-bin \
  qt6-httpserver-dev \
  qt6-serialbus-dev \
  qt6-serialport-dev \
  qt6-websockets-dev

sudo snap install flutter --classic
flutter   # init flutter first
```

```
git clone https://github.com/mincequi/ElsewhereEdge
cd ElsewhereEdge
mkdir build
cd build
cmake ..
make
```

## Auto detection
Currently, there are some auto discoveries implemented.

### SunSpec
Iotic will scan your subnet with a mask of /24. E.g. if an instance is running on a host with IPv4 address 192.168.12.34, all IP addresses from 192.168.12.1 - 192.168.12.254 will be port scanned on port 502. If a valid SunSpec header is returned, the host will be considered as a valid SunSpec device.
This incoporates a small limitation, that currently there is only **one** Modbus device per host address supported, even though Modbus TCP would support multiple SunSpec devices per IP address.

### go-e Charger

### Shelly

## MQTT API
After being connected, the discovered things will be polled and published via MQTT every 5 seconds.

The MQTT topic follows this scheme: `/elsewhere_<mac address>/<unique sunspec id>/<model id>/`.
Under this topic there are two sub-topics: `live` and `stats`. The *statistics* show the min/max values of the current day, as well as some other collected information. Thus, the *stats* topic will not be updated as often as the *live* topic, which gets an update every 3 seconds.

The payload is formatted as JSON.

## InfluxDB export
There is support for inserting data into an influxdb database. This is limited to InfluxDB 1.X on localhost at port 8086.
Support for InfluxDB 2.X with auth and remote host will follow.

## Device support
Currently, the following SunSpec models are supported: 101, 103, 160, 203. So, **ANY** solar inverter and smart meter that supports SunSpec / Modbus should also be supported here. As of now, the following devices are successfully tested:
### Solar inverters
- SMA STP8.0-3AV-40 / STP10.0-3AV-40
- SMA STP 15000TL-30 / 20000TL-30 /25000TL-30
- SMA SB 3000TL-21 / 3600TL-21 / 4000TL-21 / 5000TL-21

### Smart meters
- elgris SMART METER WiFi

### EV stations
- go-e Charger

### Smart plugs/relays
- Shelly 1 / 1PM

## Planned features
- config file
- debian package
- command line interface
- read battery inverters
- control wallboxes
