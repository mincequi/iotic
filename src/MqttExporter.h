#ifndef MQTTEXPORTER_H
#define MQTTEXPORTER_H

#include <qmqtt_client.h>

namespace sunspec {
class Model;
class StatsModel;
class SunSpecThing;
}

class MqttExporter : public QObject {
    Q_OBJECT
public:
    explicit MqttExporter(const std::string& host, uint16_t port = 1883, QObject *parent = nullptr);

    void exportLiveData(const sunspec::SunSpecThing& thing, const sunspec::Model& model);
    void exportStatsData(const sunspec::SunSpecThing& thing, const sunspec::StatsModel& model);

signals:

private:
    void onError(const QMQTT::ClientError error);

    QMQTT::Client m_client;
};

#endif // MQTTEXPORTER_H
