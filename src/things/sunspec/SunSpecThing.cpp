#include "SunSpecThing.h"

#include <regex>
#include <QDateTime>
#include <QVariant>

#include <things/sunspec/SunSpecLogger.h>
#include <things/sunspec/SunSpecModelFactory.h>
#include <things/sunspec/models/SunSpecWyeConnectMeterModelFactory.h>
#include <things/sunspec/models/SunSpecInverterModelFactory.h>
#include <things/sunspec/models/SunSpecMpptInverterExtensionModelFactory.h>

using namespace std::placeholders;

namespace sunspec {

SunSpecThing::SunSpecThing(ModbusThingPtr modbusThing)
    : Thing(*modbusThing),
    _modbusThing(modbusThing) {
    _modbusThing->stateObservable().subscribe([this](State state) {
        if (state == State::Ready && _sunSpecId.empty()) {
            pollNextUnitId();
        } else if (state == State::Failed) {
            stateSubscriber().on_next(State::Failed);
        }
    });
}

SunSpecThing::~SunSpecThing() {
}

QString SunSpecThing::host() const {
    return _modbusThing->host();
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

bool SunSpecThing::readModel(uint16_t modelId, uint32_t timestamp) {
    LOG_S(2) << this->sunSpecId() << "> reading model: " << modelId;
    if (_modelAddresses.count(modelId)) {
        return readBlock(modelId, _modelAddresses[modelId].first, _modelAddresses[modelId].second, timestamp);
    }
    return false;
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
            if (!readModel(kv.first, QDateTime::currentSecsSinceEpoch())) {
                stateSubscriber().on_next(State::Failed);
                return;
            }
            break;
        default:
            break;
        }
    }
}

uint8_t SunSpecThing::nextUnitId() {
    static const std::vector<uint8_t> ids = {
        //// Primary Devices
        /// Requirement: detect first instance for each device group (for inverters detect first two).
        // Common
        //0,                // 0 is broadcast id. Do not check
        1,                  // Always check unit id 1 first
        // Smart Meters
        240,                // Fronius
        // Solar Inverters
        126, 127,           // SMA
        100,                // Fronius
        2,                  // Solaredge
        247,                // Goodwe

        //// Secondary Devices
        /// Requirement: detect next instance for each device group (for inverters detect next two).
        // Common
        // Smart Meters
        241,                // Fronius
        // Solar Inverters
        128, 129,           // SMA
        3,                  // Fronius
        4,                  // Solaredge

        //// Tertiary Devices
        /// Detect next three instances for each device group. Four for Inverters.
        // Common
        5,
        // Smart Meters
        242, 243, 244,                  // Fronius
        // Solar Inverters
        130, 131, 132, 133, 134, 135,   // SMA
        6, 7, 8, 9,                     // Fronius
        10,                             // Solaredge

        //// The Rest
         11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
         21,  22,  23,  24,  25,  26,  27,  28,  29,  30,
         31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
         41,  42,  43,  44,  45,  46,  47,  48,  49,  50,
         51,  52,  53,  54,  55,  56,  57,  58,  59,  60,
         61,  62,  63,  64,  65,  66,  67,  68,  69,  70,
         71,  72,  73,  74,  75,  76,  77,  78,  79,  80,
         81,  82,  83,  84,  85,  86,  87,  88,  89,  90,
         91,  92,  93,  94,  95,  96,  97,  98,  99,
        101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
        111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
        121, 122, 123, 124, 125,
                                 136, 137, 138, 139, 140,
        141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
        151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
        161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
        171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
        181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
        191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
        201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
        211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
        221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
        231, 232, 233, 234, 235, 236, 237, 238, 239,
                            245, 246
    };

    // unit ids: 1 to 247
    if (_unitIdx >= ids.size()) {
        return 0;
    }

    _unitId = ids.at(_unitIdx++);
    return _unitId;
}

void SunSpecThing::pollNextUnitId() {
    const uint8_t id = nextUnitId();
    if (id == 0) {
        stateSubscriber().on_next(State::Failed);
        return;
    }

    readHeader(id);
}

void SunSpecThing::readHeader(uint8_t id) {
    const QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 40000, 4);
    auto* reply = _modbusThing->sendReadRequest(dataUnit, id);
    if  (!reply) {
        stateSubscriber().on_next(State::Failed);
    } else if (reply->isFinished()) {   // broadcast replies return immediately
        delete reply;
        stateSubscriber().on_next(State::Failed);
    } else {
        reply->connect(reply, &QModbusReply::finished, std::bind(&SunSpecThing::onReadHeader, this, reply));
    }
}

void SunSpecThing::onReadHeader(QModbusReply* reply) {
    //auto reply = qobject_cast<QModbusReply*>(sender());
    if (!reply) {
        //emit stateChanged(State::Failed);
        stateSubscriber().on_next(State::Failed);
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
            LOG_S(INFO) << "no SunSpec header found at host: " << _modbusThing->host();
            //emit stateChanged(State::Failed);
            stateSubscriber().on_next(State::Failed);
        }
    } else {
        // TODO: we have a crash here. This call arrives, although _modbusClient is already deleted.
        LOG_S(WARNING) << host().toStdString() << "> reply error: " << reply->error();
        //emit stateChanged(State::Failed);
        stateSubscriber().on_next(State::Failed);
    }

    reply->disconnect();
    reply->deleteLater();
}

void SunSpecThing::readModelTable(uint16_t address) {
    const QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, address, 2);
    auto* reply = _modbusThing->sendReadRequest(dataUnit, _unitId);
    if  (!reply) {
        //emit stateChanged(State::Failed);
        stateSubscriber().on_next(State::Failed);
    } else if (reply->isFinished()) {   // broadcast replies return immediately
        delete reply;
        //emit stateChanged(State::Failed);
        stateSubscriber().on_next(State::Failed);
    } else {
        reply->connect(reply, &QModbusReply::finished, std::bind(&SunSpecThing::onReadModelTable, this, reply));
    }
}

void SunSpecThing::onReadModelTable(QModbusReply* reply) {
    //auto reply = qobject_cast<QModbusReply*>(sender());
    if (reply->error() != QModbusDevice::NoError) {
        //emit stateChanged(State::Failed);
        stateSubscriber().on_next(State::Failed);
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
            //emit stateChanged(State::Connected);
            stateSubscriber().on_next(State::Ready);
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

bool SunSpecThing::readBlock(uint16_t modelId, uint16_t address, uint16_t length, uint32_t timestamp) {
    const QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, address, length);
    auto* reply = _modbusThing->sendReadRequest(dataUnit, _unitId);
    if  (!reply) {
        return false;
    } else if (reply->isFinished()) {   // broadcast replies return immediately
        delete reply;
        return false;
    }

    LOG_S(2) << sunSpecId() << "> reading block at: " << address;
    reply->connect(reply, &QModbusReply::finished, std::bind(&SunSpecThing::onReadBlock, this, modelId, timestamp, reply));
    return true;
}

void SunSpecThing::onReadBlock(uint16_t modelId, uint32_t timestamp, QModbusReply* reply) {
    //auto reply = qobject_cast<QModbusReply*>(sender());
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
            stateSubscriber().on_next(State::Failed);
        }
        break;
    default:
        LOG_S(WARNING) << sunSpecId() << "> error: " << reply->errorString().toStdString();
        stateSubscriber().on_next(State::Failed);
        break;
    }
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
        std::map<Property, Value> props;
        for (const auto& v : model.values()) {
            switch (v.first) {
            case DataPoint::totalActiveAcPower:
                props[Property::power] = (double)std::get<int32_t>(v.second);
                break;
            case DataPoint::operatingStatus:
                props[Property::status] = std::get<InverterOperatingStatus>(v.second);
            default:
                break;
            }
        }
        propertiesSubscriber().on_next(props);
    }
}

std::string SunSpecThing::toString(const LiveValue& v) {
    std::stringstream ss;
    ss << v;
    auto str = ss.str();
    std::transform(str.begin(), str.end(), str.begin(), [](uchar c) { return std::tolower(c); });
    str = std::regex_replace(str, std::regex("[^a-z0-9]"), "_");
    return str;
}

} // namespace sunspec
