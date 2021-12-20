#ifndef SUNSPECTHING_H
#define SUNSPECTHING_H

#include <set>

#include <QModbusTcpClient>

#include <sunspec/models/SunSpecCommonModel.h>

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

    const std::map<uint16_t, std::pair<uint16_t, uint16_t>>& models() const;

    bool connectDevice();
    void disconnectDevice();

    bool isValid() const;
    bool hasCommonModel() const;
    bool isSleeping() const;

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

    void readBlock(uint16_t modelId, uint16_t address, uint16_t size, uint32_t timestamp);
    void onReadBlock(uint16_t modelId, uint32_t timestamp);
    void onReadBlockError(uint16_t modelId, QModbusReply* reply);

    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);

    void parseModel(uint16_t modelId, const std::vector<uint16_t>& buffer, uint32_t timestamp);

    QModbusTcpClient m_modbusClient;
    uint8_t m_unitId = 0;
    uint8_t m_unitIdx = 0;
    std::string m_sunSpecId;

    uint8_t m_timeoutCount = 0;

    std::map<uint16_t, std::pair<uint16_t, uint16_t>> m_modelAddresses;

    std::map<uint16_t, sunspec::Model> m_models;
};

} // namespace sunspec

#endif // SUNSPECTHING_H
