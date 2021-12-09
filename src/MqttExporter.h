#ifndef MQTTEXPORTER_H
#define MQTTEXPORTER_H

#include <qmqtt_client.h>

#include "sunspec/SunSpecModel.h"

class SunSpecThing;

class MqttExporter : public QObject {
    Q_OBJECT
public:
    explicit MqttExporter(const std::string& host, uint16_t port = 1883, QObject *parent = nullptr);

    void exportSunSpecModel(SunSpecThing* thing, const SunSpecModel& model);

signals:

private:
    void onError(const QMQTT::ClientError error);

    QMQTT::Client m_client;
};

#endif // MQTTEXPORTER_H
