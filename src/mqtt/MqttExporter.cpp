#include "MqttExporter.h"

#include <qmqtt_message.h>
#include <common/Util.h>
#include <iot/sunspec/SunSpecLogger.h>
#include <iot/sunspec/SunSpecModel.h>
#include <iot/sunspec/SunSpecStatsModel.h>
#include <iot/sunspec/SunSpecThing.h>

static quint16 s_msgId = 0;

MqttExporter::MqttExporter(const std::string& host, uint16_t port, QObject *parent) :
    QObject(parent),
    m_client(QString::fromStdString(host), port, false, false) {

    QObject::connect(&m_client, &QMQTT::Client::error, this, &MqttExporter::onError);
    m_client.connectToHost();

    LOG_S(INFO) << "publishing data to host: " << host << ", under topic: " << "elsewhere_" + util::getMacAddress().remove(':');
}

MqttExporter::~MqttExporter() {
}

void MqttExporter::exportLiveData(const sunspec::SunSpecThing& thing, const sunspec::Model& model) {
    if (!m_client.isConnectedToHost()) {
        m_client.connectToHost();
    }

    QString topic = "elsewhere_" + util::getMacAddress().remove(':')
                    + "/" + QString::fromStdString(thing.manufacturer())
                    + "/" + QString::fromStdString(thing.product())
                    + "/" + QString::fromStdString(thing.serial());
    if (model.modelId() == 160) topic += "/mppt";
    topic += "/live";

    std::stringstream ss;
    ss << model;

    QMQTT::Message message(++s_msgId,
                           topic,
                           QByteArray::fromStdString(ss.str()),
                           0,
                           true);

    LOG_F(1, "publishing topic: {}, payload size: {} bytes", topic.toStdString().c_str(), ss.str().size());
    m_client.publish(message);
}

void MqttExporter::exportStatsData(const sunspec::SunSpecThing& thing, const sunspec::StatsModel& model) {
    if (!m_client.isConnectedToHost()) {
        m_client.connectToHost();
    }

    QString topic = "elsewhere_" + util::getMacAddress().remove(':')
                    + "/" + QString::fromStdString(thing.manufacturer())
                    + "/" + QString::fromStdString(thing.product())
                    + "/" + QString::fromStdString(thing.serial());
    if (model.modelId() == 160) topic += "/mppt";
    topic += "/stats";

    std::stringstream ss;
    ss << model;

    QMQTT::Message message(++s_msgId,
                           topic,
                           QByteArray::fromStdString(ss.str()),
                           0,
                           true);

    LOG_F(1, "publishing topic: {}, payload size: {} bytes", topic.toStdString().c_str(), ss.str().size());
    m_client.publish(message);
}

void MqttExporter::onError(const QMQTT::ClientError error) {
    LOG_S(WARNING) << "client error: " << error;
}
