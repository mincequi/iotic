# iotic

<img height="480" src="https://github.com/mincequi/iotic/assets/1805183/cba31985-6c84-4bd5-ac69-3322199a25e0"> <img height="480" src="https://github.com/mincequi/iotic/assets/1805183/72d89e72-f6ea-4acf-84db-8b53d311a39a">

Iotic is a multi-purpose IoT device manager which focuses on electric and thermal energy management.
Its main feature is pv surplus charging of your electric vehicle.

# Features
## Device support
### Solar inverters
Currently, the following SunSpec models are supported: 101, 103, 160, 203. So, **ANY** solar inverter and smart meter that supports SunSpec / Modbus should also be supported here. As of now, the following devices are successfully tested:
- SMA STP8.0-3AV-40 / STP10.0-3AV-40
- SMA STP 15000TL-30 / 20000TL-30 /25000TL-30
- SMA SB 3000TL-21 / 3600TL-21 / 4000TL-21 / 5000TL-21

### Smart meters
- elgris SMART METER WiFi

### EV stations
- go-e Charger

### Smart plugs/relays
- Shelly 1 / 1PM

## Auto detection
All supporterd things are automatically discovered using different technologies:

### Modbus SunSpec devices
Iotic will scan your subnet with a mask of /24. E.g. if an instance is running on a host with IPv4 address 192.168.12.34, all IP addresses from 192.168.12.1 - 192.168.12.254 will be port scanned on port 502. If a valid SunSpec header is returned, the host will be considered as a valid SunSpec device.
This incoporates a small limitation, that currently there is only **one** Modbus device per host address supported, even though Modbus TCP would support multiple SunSpec devices per IP address.

### HTTP devices
Devices with an HTTP API are discovered using mDNS service discovery (like go-eCharger and Shelly).

## PV surplus electric vehicle charging
iotic implements several strategies to realize PC surplus charging of electric vehicles. It will auto-adapt the charging power to PV excess.

# Requirements
- C++17
- Qt5 or Qt6: Core Network SerialBus
- Flutter

# Installation (Debian/Ubuntu)
## Raspberry Pi OS
You need at least Bookworm (not working with Bullseye).
Installation
```
wget https://github.com/mincequi/iotic/releases/download/v0.3.0/iotic_0.3.0_armhf.deb
sudo apt install ./iotic_0.3.0_armhf.deb
```
Starting
```
sudo systemctl start iotic.service               # start-up service.
sudo systemctl enable iotic.service              # start-up service on each reboot.
```

# Compiling (Debian/Ubuntu)
## Prerequisites
```
sudo apt install \
  build-essential \
  cmake \
  git \
  snapd \
  libcurl4-openssl-dev \
  libqt6serialbus6-bin \
  qt6-serialbus-dev \
  qt6-serialport-dev

sudo snap install flutter --classic
flutter   # init flutter first
```

```
git clone https://github.com/mincequi/iotic
cd iotic/src/webapp
flutter build web --web-renderer html
cd ../..
mkdir build
cd build
cmake ..
make
```

# MQTT API
After being connected, the discovered things will be polled and published via MQTT every 5 seconds.

The MQTT topic follows this scheme: `/iotic_<mac address>/<unique sunspec id>/<model id>/`.
Under this topic there are two sub-topics: `live` and `stats`. The *statistics* show the min/max values of the current day, as well as some other collected information. Thus, the *stats* topic will not be updated as often as the *live* topic, which gets an update every 3 seconds.

The payload is formatted as JSON.

# Planned features
- command line interface
- read battery inverters
- InfluxDB
