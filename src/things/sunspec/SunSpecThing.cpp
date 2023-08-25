#include "SunSpecThing.h"

#include <regex>
#include <QDateTime>
#include <QVariant>

#include <things/sunspec/SunSpecLogger.h>
#include <things/sunspec/SunSpecModelFactory.h>
#include <things/sunspec/models/SunSpecWyeConnectMeterModelFactory.h>
#include <things/sunspec/models/SunSpecInverterModelFactory.h>
#include <things/sunspec/models/SunSpecMpptInverterExtensionModelFactory.h>

namespace sunspec {

SunSpecThing::SunSpecThing(const ThingInfo& info)
    : Thing{info} {
    connect(&_modbusClient, &QModbusTcpClient::stateChanged, this, &SunSpecThing::onStateChanged);
    connect(&_modbusClient, &QModbusTcpClient::errorOccurred, this, &SunSpecThing::onErrorOccurred);

    _modbusClient.setConnectionParameter(QModbusDevice::NetworkPortParameter, 502);
    _modbusClient.setConnectionParameter(QModbusDevice::NetworkAddressParameter, QString::fromStdString(info.host()));

    _modbusClient.setTimeout(2000);
    _modbusClient.setNumberOfRetries(1);
}

ThingPtr SunSpecThing::from(const ThingInfo& thingInfo) {
    return ThingPtr(new SunSpecThing(thingInfo));
}

QString SunSpecThing::host() const {
    return _modbusClient.connectionParameter(QModbusDevice::NetworkAddressParameter).toString();
}

uint8_t SunSpecThing::modbusUnitId() const {
    return _unitId;
}

std::string SunSpecThing::sunSpecId() const {
    return _sunSpecId;
}

std::string SunSpecThing::manufacturer() const {
    return _manufacturer;
}

std::string SunSpecThing::product() const {
    return _product;
}

std::string SunSpecThing::serial() const {
    return _serial;
}

const std::map<uint16_t, std::pair<uint16_t, uint16_t>>& SunSpecThing::models() const {
    return _modelAddresses;
}

bool SunSpecThing::connectDevice() {
    return _modbusClient.connectDevice();
}

void SunSpecThing::disconnectDevice() {
    return _modbusClient.disconnectDevice();
}

bool SunSpecThing::isValid() const {
    return _modbusClient.state() == QModbusDevice::State::ConnectedState;
}

bool SunSpecThing::isSleeping() const {
    for (const auto& kv : _models) {
        if (kv.second.values().count(sunspec::operatingState) &&
                kv.second.values().count(sunspec::operatingState) == InverterOperatingState::sleeping) {
            return true;
        }
    }

    return false;
}

void SunSpecThing::readModel(uint16_t modelId, uint32_t timestamp) {
    LOG_S(2) << this->sunSpecId() << "> reading model: " << modelId;
    if (_modelAddresses.count(modelId)) {
        readBlock(modelId, _modelAddresses[modelId].first, _modelAddresses[modelId].second, timestamp);
    }
}

void SunSpecThing::reset() {
    _models.clear();
}

void SunSpecThing::doRead() {
    for (const auto& kv : _modelAddresses) {
        switch (kv.first) {
        case Model::Id::InverterSinglePhase:
        case Model::Id::InverterThreePhase:
        case Model::Id::InverterSplitPhase:
        case Model::Id::MeterWyeConnectThreePhase:
            readModel(kv.first, QDateTime::currentSecsSinceEpoch());
            break;
        default:
            break;
        }
    }
}

uint8_t SunSpecThing::nextUnitId() {
    // unit ids: 1 to 247
    if (_unitId > 246) {
        return 0;
    }

    static const std::vector<uint8_t> ids = {
        1,   2,   3,   4,   5,    // Any Modbus device
        126, 127, 128, 129, 130,    // SMA Solar Inverter
        100,                        // Fronius Solar Inverter
        240, 241, 242, 243, 244,    // Fronius Smart Meter

        6, 7, 8, 9, 10, 11, 12, 13, 14, 15, // Any Modbus device
        131, 132, 132, 133, 134, 135,       // SMA Solar Inverter

        // Any Modbus devices
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
        46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
        56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
        66, 67, 68, 69, 70, 71, 72, 73, 74, 75,
        76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
        86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
        96, 97, 98, 99,

        // SMA Solar inverters
        136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
        146, 147, 148, 149, 150,

        // Fronius String Controls
        101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
        111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
        121, 122, 123, 124, 125,

        // Any Modbus devices
        151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
        161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
        171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
        181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
        191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
        201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
        211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
        221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
        231, 232, 233, 234, 235, 236, 237, 238, 239,

        // Fronius Sensor Card
        245, 246, 247               // Last items to check
    };

    _unitId = ids.at(_unitIdx++);
    return _unitId;
}

void SunSpecThing::pollNextUnitId() {
    const uint8_t id = nextUnitId();
    if (id == 0) {
        emit stateChanged(State::Failed);
        return;
    }

    readHeader(id);
}

void SunSpecThing::readHeader(uint8_t id) {
    const QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 40000, 4);
    auto* reply = _modbusClient.sendReadRequest(dataUnit, id);
    if  (!reply) {
        emit stateChanged(State::Failed);
    } else if (reply->isFinished()) {   // broadcast replies return immediately
        delete reply;
        emit stateChanged(State::Failed);
    } else {
        connect(reply, &QModbusReply::finished, this, &SunSpecThing::onReadHeader);
    }
}

void SunSpecThing::onReadHeader() {
    auto reply = qobject_cast<QModbusReply*>(sender());
    if (!reply) {
        emit stateChanged(State::Failed);
        return;
    }

    if (reply->error() == QModbusDevice::ProtocolError) {
        pollNextUnitId();
    } else if (reply->error() == QModbusDevice::NoError) {
        //qInfo() << "Host:" << _modbusClient.connectionParameter(QModbusDevice::NetworkAddressParameter).toString();
        const QModbusDataUnit unit = reply->result();
        if (unit.value(0) == 0x5375 &&
                unit.value(1) == 0x6E53 &&
                unit.value(2) == 1 &&
                (unit.value(3) == 65 || unit.value(3) == 66)) {
            readBlock(1, 40004, unit.value(3), 0);
            readModelTable(40004 + unit.value(3));
        } else {
            LOG_S(INFO) << "no SunSpec header found at host: " << _modbusClient.connectionParameter(QModbusDevice::NetworkAddressParameter).toString();;
            emit stateChanged(State::Failed);
        }
    } else {
        LOG_S(WARNING) << host().toStdString() << "> reply error: " << reply->error();
        emit stateChanged(State::Failed);
    }

    reply->deleteLater();
}

void SunSpecThing::readModelTable(uint16_t address) {
    const QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, address, 2);
    auto* reply = _modbusClient.sendReadRequest(dataUnit, _unitId);
    if  (!reply) {
        emit stateChanged(State::Failed);
    } else if (reply->isFinished()) {   // broadcast replies return immediately
        delete reply;
        emit stateChanged(State::Failed);
    } else {
        connect(reply, &QModbusReply::finished, this, &SunSpecThing::onReadModelTable);
    }
}

void SunSpecThing::onReadModelTable() {
    auto reply = qobject_cast<QModbusReply*>(sender());
    if (reply->error() != QModbusDevice::NoError) {
        emit stateChanged(State::Failed);
    } else {
        const QModbusDataUnit unit = reply->result();
        addModelAddress(unit.value(0), unit.startAddress() + 2, unit.value(1));
        // Check if we finished reading model table
        if (unit.value(0) == 0xFFFF) {
            for (const auto& kv : _modelAddresses) {
                if (kv.first >= Model::Id::InverterSinglePhase && kv.first < Model::Id::InverterMpptExtension) {
                    _type = Type::SolarInverter;
                    break;
                } else if (kv.first >= Model::Id::MeterSinglePhase && kv.first <= Model::Id::MeterWyeConnectThreePhase) {
                    _type = Type::SmartMeter;
                    break;
                }
            }
            emit stateChanged(State::Connected);
        } else {
            readModelTable(unit.startAddress() + 2 + unit.value(1));
        }
    }

    reply->deleteLater();
}

void SunSpecThing::addModelAddress(uint16_t modelId, uint16_t startAddress, uint16_t length) {
    // Special handling for blocks, that do not fit into a 256 bytes payload (modbus tcp limit).
    //if (modelId == sunspec::Model::Id::InverterMpptExtension && length > 125) {
    //    length = 110;
    //}

    // Special handling for SMA solar inverters: we only request first two MPPs
    if (modelId == Model::Id::InverterMpptExtension) {
        length = std::min(length, (uint16_t)48);
    }
    // Tried to tune inverter requests, however reducing amount of registers did not help.
    /*else if (modelId == 103) {
        startAddress += 12;
        length = 2;
    }*/
    _modelAddresses[modelId] = { startAddress, length };
}

void SunSpecThing::readBlock(uint16_t modelId, uint16_t address, uint16_t length, uint32_t timestamp) {
    const QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, address, length);
    auto* reply = _modbusClient.sendReadRequest(dataUnit, _unitId);
    if  (!reply) {
        emit stateChanged(State::Failed);
    } else if (reply->isFinished()) {   // broadcast replies return immediately
        delete reply;
        emit stateChanged(State::Failed);
    } else {
        LOG_S(2) << sunSpecId() << "> reading block at: " << address;
        connect(reply, &QModbusReply::finished, this, std::bind(&SunSpecThing::onReadBlock, this, modelId, timestamp));
    }
}

void SunSpecThing::onReadBlock(uint16_t modelId, uint32_t timestamp) {
    auto reply = qobject_cast<QModbusReply*>(sender());
    if (reply->error() != QModbusDevice::NoError) {
        onReadBlockError(modelId, reply);
    } else {
        // Note: you have to instantiate the values locally. Qt containers do weird things.
        const auto values = reply->result().values();
        const std::vector<uint16_t> buffer(values.begin(), values.end());
        parseModel(modelId, buffer, timestamp);
        _timeoutCount = 0;
    }

    reply->deleteLater();
}

void SunSpecThing::onReadBlockError(uint16_t modelId, QModbusReply* reply) {
    switch (reply->error()) {
    case QModbusDevice::TimeoutError:
        LOG_S(WARNING) << sunSpecId() << "> timeout reading block: " << modelId;
        ++_timeoutCount;
        // Set device as failed if sunSpecId is empty or it timed out 5 times in a row.
        if (_timeoutCount > 4 || sunSpecId().empty()) {
            emit stateChanged(State::Failed);
        }
        break;
    default:
        LOG_S(WARNING) << sunSpecId() << "> error: " << reply->errorString().toStdString();
        emit stateChanged(State::Failed);
        break;
    }
}

void SunSpecThing::onStateChanged(QModbusDevice::State state) {
    if (state == QModbusDevice::State::ConnectedState && _sunSpecId.empty()) {
        LOG_S(INFO) << "modbus host found: " << _modbusClient.connectionParameter(QModbusDevice::NetworkAddressParameter).toString();
        pollNextUnitId();
    } else if (state == QModbusDevice::State::UnconnectedState && !_sunSpecId.empty()) {
        // Elgris smart meters disconnects after 10s. So, we automatically reconnect.
        connectDevice();
    }
}

void SunSpecThing::onErrorOccurred(QModbusDevice::Error error) {
    if (error == QModbusDevice::Error::NoError) {
        return;
    }

    if (_sunSpecId.empty()) {
        emit stateChanged(State::Failed);
        return;
    }

    // We filter for connection error, because remotes might hang up.
    LOG_IF_S(WARNING, error != QModbusDevice::Error::ConnectionError) << _sunSpecId << "> error occured: " << _modbusClient.errorString().toStdString();
}

void SunSpecThing::parseModel(uint16_t modelId, const std::vector<uint16_t>& buffer, uint32_t timestamp) {
    if (modelId == 1) {
        auto& commonModel = _models[modelId];
        if (SunSpecCommonModelFactory::updateFromBuffer(commonModel, buffer, timestamp)) {
            _manufacturer = toString(commonModel.values().at(Manufacturer));
            _product = toString(commonModel.values().at(Product));
            _serial = toString(commonModel.values().at(Serial));
            _sunSpecId = _manufacturer + "_" + _product + "_" + _serial;
            _discoveryType = Thing::DiscoveryType::SunSpec;
        }
    } else if (sunspec::ModelFactory::updateFromBuffer(_models, modelId, buffer, timestamp)) {
        const auto& model = _models[modelId];
        for (const auto& v : model.values()) {
            switch (v.first) {
            case DataPoint::totalActiveAcPower:
                _propertiesSubject.get_subscriber().on_next({{ReadableThingProperty::power, std::get<int32_t>(v.second)}});
                break;
            default:
                break;
            }
        }
        emit modelRead(model, timestamp);
    }
}

std::string SunSpecThing::toString(const LiveValue& v) {
    std::stringstream ss;
    ss << v;
    auto str = ss.str();
    std::transform(str.begin(), str.end(), str.begin(), [](uchar c) { return std::tolower(c); });
    std::replace(str.begin(), str.end(), ' ', '-');
    std::replace(str.begin(), str.end(), '#', '-');
    std::replace(str.begin(), str.end(), '+', '-');
    std::replace(str.begin(), str.end(), '$', '-');
    std::replace(str.begin(), str.end(), '.', '-');
    std::replace(str.begin(), str.end(), ':', '-');
    std::replace(str.begin(), str.end(), ',', '-');
    std::replace(str.begin(), str.end(), ';', '-');
    std::replace(str.begin(), str.end(), '!', '-');
    std::replace(str.begin(), str.end(), '?', '-');

    return str;
}

} // namespace sunspec
