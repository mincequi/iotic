#include "ModbusThing.h"

#include <QModbusTcpClient>

#include <common/Logger.h>

using namespace std::placeholders;

ModbusThing::ModbusThing(const ThingInfo& info) :
    Thing(info),
    _modbusClient(new QModbusTcpClient) {

    _modbusClient->connect(_modbusClient, &QModbusTcpClient::stateChanged, std::bind(&ModbusThing::onStateChanged, this, _1));
    _modbusClient->connect(_modbusClient, &QModbusTcpClient::errorOccurred, std::bind(&ModbusThing::onErrorOccurred, this, _1));

    _modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, 502);
    _modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, QString::fromStdString(info.host()));

    _modbusClient->setTimeout(5000);
    _modbusClient->setNumberOfRetries(2);
}

ModbusThing::~ModbusThing() {
    _modbusClient->disconnect();
    _modbusClient->deleteLater();
}

bool ModbusThing::connect() {
    return _modbusClient->connectDevice();
}

void ModbusThing::disconnect() {
    return _modbusClient->disconnectDevice();
}

void ModbusThing::doRead() {
};

void ModbusThing::onStateChanged(QModbusDevice::State state_) {
    if (state_ == QModbusDevice::State::ConnectedState && state() == Thing::State::Uninitialized) {
        LOG_S(INFO) << "host found: " << _modbusClient->connectionParameter(QModbusDevice::NetworkAddressParameter).toString();
        stateSubscriber().on_next(State::Ready);
        //pollNextUnitId();
    } else if (state_ == QModbusDevice::State::UnconnectedState && state() != Thing::State::Uninitialized) {
        // Elgris smart meters disconnects after 10s. So, we automatically reconnect.
        LOG_S(WARNING) << id() << "> disconnected. Reconnecting...";
        connect();
    }
}

void ModbusThing::onErrorOccurred(QModbusDevice::Error error) {
    if (error == QModbusDevice::Error::NoError) {
        return;
    }

    if (state() == Thing::State::Uninitialized) {
        stateSubscriber().on_next(State::Failed);
        return;
    }

    // We filter for connection error, because remotes might hang up.
    LOG_IF_S(WARNING, error != QModbusDevice::Error::ConnectionError) << host() << "> error occured: " << _modbusClient->errorString().toStdString();
}
