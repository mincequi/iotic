---
layout: page
title: Key Concepts
nav_order: 20
---

## Discovery

Automatic device discovery in the context of IoT (Internet of Things)
streamlines the process by which IoT devices seamlessly join a management system
(like Iotic) without manual intervention.
This system ensures easy detection of devices and device changes
and contributes to the overall efficiency and reliability. With plug-and-play
functionality and centralized monitoring, automatic device
discovery simplifies the integration and management of devices within the Iotic
ecosystem. Iotic dynamically identifies and updates
its inventory in real-time.

### Modbus Discovery

Iotic has a built-in modbus device discovery. This process involves a focused
port scan within the same IPv4 subnet of an Iotic instance by probing devices
for open port 502, commonly associated with Modbus communication.

### SunSpec Discovery

SunSpec discovery builds upon Modbus discovery by specifically targeting SunSpec
information models within the Modbus protocol. By honing in on SunSpec data
structures, Iotic identifies and integrates devices adhering to SunSpec
standards for renewable energy, offering a more nuanced and comprehensive
approach to recognition and integration within the Iotic ecosystem.

### HTTP Discovery

This discovery automates the identification and accessibility of HTTP-based
services within a network, considering that certain IoT devices, including
go-eChargers, utilize this protocol. This method allows devices hosting such
services to broadcast their presence.

## Things

In the context of the Internet of Things (IoT), everyday objects and devices are
interconnected, enabling them to communicate and share data over a network. This
connectivity allows for seamless integration and automation, enhancing
efficiency and convenience in various aspects of life. From smart homes with
connected thermostats and applications like optimizing energy production and
monitoring performance in photovoltaic systems, the IoT revolutionizes how we
interact with and manage the physical world through the power of interconnected
technology.

## Properties

In the context of an IoT device or thing, "properties" refer to the
characteristics, attributes, or data points associated with that device.
Properties provide information about the state, behavior, or configuration of an
IoT device, allowing users and other systems to interact with and monitor the
device effectively. 
