#include "MqttExporter.h"

#include <qmqtt_message.h>
#include <common/Util.h>
#include <things/sunspec/SunSpecLogger.h>
#include <things/sunspec/SunSpecModel.h>
#include <things/sunspec/SunSpecStatsModel.h>
#include <things/sunspec/SunSpecThing.h>

static quint16 s_msgId = 0;

MqttExporter::MqttExporter(const std::string& host, uint16_t port, QObject *parent) :
    QObject(parent),
    _client(QString::fromStdString(host), port, false, false) {

    QObject::connect(&_client, &QMQTT::Client::error, this, &MqttExporter::onError);
    _client.connectToHost();

    _topic = "iotic_" + util::getMacAddress().remove(':');
    LOG_S(INFO) << "host: " << host << ", topic: " << _topic;
}

MqttExporter::~MqttExporter() {
}

void MqttExporter::exportLiveData(const sunspec::SunSpecThing& thing, const sunspec::Model& model) {
    if (!_client.isConnectedToHost()) {
        _client.connectToHost();
    }

    QString topic = _topic
                    + "/" + QString::fromStdString(thing.manufacturer())
                    + "/" + QString::fromStdString(thing.product())
                    + "/" + QString::fromStdString(thing.serial());
    if (model.modelId() == sunspec::Model::Id::InverterMpptExtension) topic += "/mppt";
    topic += "/live";

    std::stringstream ss;
    ss << model;

    QMQTT::Message message(++s_msgId,
                           topic,
                           QByteArray::fromStdString(ss.str()),
                           0,
                           true);

    LOG_F(1, "publishing topic: {}, payload size: {} bytes", topic.toStdString().c_str(), ss.str().size());
    _client.publish(message);
}

void MqttExporter::exportStatsData(const sunspec::SunSpecThing& thing, const sunspec::StatsModel& model) {
    if (!_client.isConnectedToHost()) {
        _client.connectToHost();
    }

    QString topic = _topic
                    + "/" + QString::fromStdString(thing.manufacturer())
                    + "/" + QString::fromStdString(thing.product())
                    + "/" + QString::fromStdString(thing.serial());
    if (model.modelId() == sunspec::Model::Id::InverterMpptExtension) topic += "/mppt";
    topic += "/stats";

    std::stringstream ss;
    ss << model;

    QMQTT::Message message(++s_msgId,
                           topic,
                           QByteArray::fromStdString(ss.str()),
                           0,
                           true);

    LOG_F(1, "publishing topic: {}, payload size: {} bytes", topic.toStdString().c_str(), ss.str().size());
    _client.publish(message);
}

void MqttExporter::onError(const QMQTT::ClientError error) {
    LOG_S(WARNING) << "client error: " << error;
}
