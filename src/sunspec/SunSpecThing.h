#ifndef SUNSPECTHING_H
#define SUNSPECTHING_H

#include <QModbusTcpClient>

#include "SunSpecCommonModel.h"

#include "SunSpecWyeConnectMeterModelFactory.h"
#include "SunSpecInverterModelFactory.h"
#include "SunSpecMpptInverterExtensionModelFactory.h"

namespace sunspec {

class SunSpecThing : public QObject {
    Q_OBJECT

public:
    enum class State {
        Connected,
        Failed
    };

    SunSpecThing(const QString& host, uint16_t port = 502);

    QString host() const;
    uint8_t modbusUnitId() const;

    std::string sunSpecId() const;

    std::optional<SunSpecCommonModel> commonModel;
    std::optional<sunspec::Model> meterModel;
    std::optional<sunspec::Model> inverterModel;
    std::optional<sunspec::Model> inverterExtensionModel;

    QList<uint16_t> models() const;

    QMap<uint16_t, QVector<uint16_t>> blocks() const;

    bool connectDevice();
    void disconnectDevice();

    bool isValid() const;

    void readModel(uint16_t modelId, uint32_t timestamp);

    void reset();

signals:
    void stateChanged(State state);
    void modelRead(const sunspec::Model& model);

private:
    uint8_t nextUnitId();
    void pollNextUnitId();

    void readHeader(uint8_t id);
    void onReadHeader();

    void readModelTable(uint16_t address);
    void onReadModelTable();
    void addModelAddress(uint16_t modelId, uint16_t startAddress, uint16_t length);

    void readBlock(uint16_t address, uint16_t size, uint32_t timestamp);
    void onReadBlock(uint32_t timestamp);
    void onReadBlockError(QModbusReply* reply);

    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);

    void parseModel(const std::vector<uint16_t>& buffer, uint32_t timestamp);

    QModbusTcpClient m_modbusClient;
    uint8_t m_unitId = 0;
    uint8_t m_unitIdx = 0;
    std::string m_sunSpecId;

    uint8_t m_timeoutCount = 0;

    QMap<uint16_t, std::pair<uint16_t, uint16_t>> m_modelAddresses;
    QMap<uint16_t, QVector<uint16_t>> m_blocks;
};

} // namespace sunspec

#endif // SUNSPECTHING_H
