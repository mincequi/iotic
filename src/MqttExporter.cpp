#include "MqttExporter.h"

#include <loguru.hpp>

#include <qmqtt_message.h>

#include "Util.h"
#include "sunspec/SunSpecModel.h"
#include "sunspec/SunSpecThing.h"

static quint16 msgId = 0;

MqttExporter::MqttExporter(const std::string& host, uint16_t port, QObject *parent) :
    QObject(parent),
    m_client(QString::fromStdString(host), port, false, false) {

    QObject::connect(&m_client, &QMQTT::Client::error, this, &MqttExporter::onError);
    m_client.connectToHost();
}

void MqttExporter::exportSunSpecModel(SunSpecThing* thing, const SunSpecModel& model) {
    if (!m_client.isConnectedToHost()) {
        m_client.connectToHost();
    }

    QString topic = "elsewhere/" + util::getMacAddress().toLower() + "/" + QString::fromStdString(thing->sunSpecId());
    topic += "/live";

    QMQTT::Message message(++msgId,
                           topic,
                           "hello world", //QByteArray::fromRawData(reinterpret_cast<const char*>(data.data()), data.size()),
                           0,
                           true);

    LOG_F(1, "publishing topic: %s, payload size: %zu bytes", topic.toStdString().c_str(), 2);
    m_client.publish(message);
}

void MqttExporter::onError(const QMQTT::ClientError error) {
    LOG_S(WARNING) << "client error: " << error;
}
