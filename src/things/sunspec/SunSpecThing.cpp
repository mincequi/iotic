#include "SunSpecThing.h"

#include <regex>

#include <rpp/operators/filter.hpp>

#include <things/sunspec/SunSpecLogger.h>
#include <things/sunspec/SunSpecModelFactory.h>
#include <things/sunspec/models/SunSpecCommonModelFactory.h>
#include <things/sunspec/models/SunSpecInverterModelFactory.h>
#include <things/sunspec/models/SunSpecMpptInverterExtensionModelFactory.h>
#include <things/sunspec/models/SunSpecWyeConnectMeterModelFactory.h>

using namespace std::placeholders;

namespace sunspec {

SunSpecThing::SunSpecThing(ModbusThingPtr_asio modbusThing)
    : Thing(*modbusThing),
    _modbusThing(modbusThing) {

    // TODO: move polling for unitId out of this class into SunSpecDiscovery
    // We receive a ready/connected modbusThing. Start checking for SunSpec header.
    pollNextUnitId();

    /*
    _modbusThing->stateObservable().filter([](State state) {
        return state == State::Failed;
    }).subscribe(stateSubscriber());
    */

    _modbusThing->exceptionObservable().subscribe([this](int /*exception*/) {
        if (_sunSpecId.empty()) {
            pollNextUnitId();
        }
    });

    //
    _modbusThing->holdingRegistersObservable().map([this](ModbusResponse response) {
        if (response.userData == ReadHeader) {
            return onReadHeader(response);
        } else if (response.values.size() == 2) {
            return onReadModelEntry(response);
        } else {
            onReadBlock(response);
        }
        return std::optional<State>();
    }).filter([](const std::optional<State>& state) {
        return state.has_value();
    }).map([](const std::optional<State>& state) {
        return state.value();
    }).subscribe(stateSubscriber());
}

SunSpecThing::~SunSpecThing() {
    LOG_S(2) << id() << "> destroyed";
}

std::string SunSpecThing::host() const {
    return _modbusThing->host();
}

uint8_t SunSpecThing::unitId() const {
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
    return _blockAddresses;
}

bool SunSpecThing::readModel(uint16_t modelId) {
    LOG_S(2) << this->sunSpecId() << "> reading model: " << modelId;
    if (_blockAddresses.count(modelId)) {
        readBlock(modelId, _blockAddresses[modelId].first, _blockAddresses[modelId].second);
        return true;
    }
    return false;
}

void SunSpecThing::doRead() {
    for (const auto& kv : _blockAddresses) {
        switch (kv.first) {
        case Model::Id::InverterSinglePhase:
        case Model::Id::InverterThreePhase:
        case Model::Id::InverterSplitPhase:
        case Model::Id::MeterWyeConnectThreePhase:
            readModel(kv.first);
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
        LOG_S(INFO) << host() << "> all ids checked. No SunSpec device found";
        stateSubscriber().on_next(State::Failed);
        return;
    }

    readHeader(id);
}

void SunSpecThing::readHeader(uint8_t id) {
    // Read entry point of sunspec.
    // First 2 registers form the well-known value 'SunS'(0x53756E53).
    // Next 2 registers if the common model (1) followed by its size (65 or 66).
    _modbusThing->readHoldingRegisters(id, 40000, 4, ReadHeader);
}

std::optional<Thing::State> SunSpecThing::onReadHeader(const ModbusResponse& response) {
    _headerLength = response.values[3];
    if (response.values[0] == 0x5375 && response.values[1] == 0x6E53 &&
            response.values[2] == 1 && (_headerLength == 65 || _headerLength == 66)) {
        LOG_S(INFO) << host() << "> sunspec device at unitId: " << (int)response.unitId;

        // Read (rest of) common model
        readBlock(1, 40004, _headerLength);
    } else {
        return State::Failed;
    }
    return {};
}

void SunSpecThing::readModelEntry(uint16_t address) {
    _modbusThing->readHoldingRegisters(_unitId, address, 2, address);
}

std::optional<Thing::State> SunSpecThing::onReadModelEntry(const ModbusResponse& response) {
    // Check if we finished reading model table
    if (response.values[0] != 0xFFFF) {
        addModelAddress(response.values[0], response.userData + 2, response.values[1]);
        readModelEntry(response.userData + 2 + response.values[1]);
    } else {
        for (const auto& kv : _blockAddresses) {
            if (kv.first >= Model::Id::InverterSinglePhase && kv.first < Model::Id::InverterMpptExtension) {
                _type = Type::SolarInverter;
                break;
            } else if (kv.first >= Model::Id::MeterSinglePhase && kv.first <= Model::Id::MeterWyeConnectThreePhase) {
                _type = Type::SmartMeter;
                break;
            }
        }
        return State::Ready;
    }
    return {};
}

void SunSpecThing::addModelAddress(uint16_t modelId, uint16_t startAddress, uint16_t length) {
    // Special handling for blocks, that do not fit into a 256 bytes payload (modbus tcp limit).
    //if (modelId == sunspec::Model::Id::InverterMpptExtension && length > 125) {
    //    length = 110;
    //}

    LOG_S(1) << "model: " << modelId << ", address: " << startAddress << ", length: " << length;
    // Special handling for SMA solar inverters: we only request first two MPPs
    if (modelId == Model::Id::InverterMpptExtension) {
        length = std::min(length, (uint16_t)48);
    }
    _blockAddresses[modelId] = { startAddress, length };
}

void SunSpecThing::readBlock(uint16_t modelId, uint16_t address, uint16_t length) {
    // Length == 2 means we are reading the model table.
    if (length == 2) {
        LOG_S(WARNING) << "> not reading block with size 2";
        return;
    }
    _modbusThing->readHoldingRegisters(_unitId, address, length, modelId);
    LOG_S(2) << sunSpecId() << "> reading block at: " << address;
}

void SunSpecThing::onReadBlock(const ModbusResponse& response) {
    parseBlock(response.userData, response.values);
    _timeoutCount = 0;

    // Read next model entry
    if (response.userData == 1)
        readModelEntry(40004 + _headerLength);
}

void SunSpecThing::parseBlock(uint16_t modelId, const std::vector<uint16_t>& buffer) {
    if (modelId == 1) {
        auto& commonModel = _models[modelId];
        if (SunSpecCommonModelFactory::updateFromBuffer(commonModel, buffer)) {
            _manufacturer = toString(commonModel.values().at(Manufacturer));
            _product = toString(commonModel.values().at(Product));
            _serial = toString(commonModel.values().at(Serial));
            _sunSpecId = _manufacturer + "_" + _product + "_" + _serial;
            _discoveryType = Thing::DiscoveryType::SunSpec;
        }
    } else if (sunspec::ModelFactory::updateFromBuffer(_models, modelId, buffer)) {
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
