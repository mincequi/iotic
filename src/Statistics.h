#pragma once

#include <QObject>

#include <things/sunspec/SunSpecModel.h>
#include <things/sunspec/SunSpecStatsModel.h>

namespace sunspec {
class SunSpecThing;
}

class Statistics : public QObject {
    Q_OBJECT
public:
    explicit Statistics(QObject *parent = nullptr);

    void reset();

    void feedModel(const sunspec::SunSpecThing& thing, const sunspec::Model& model);

signals:
    void statsChanged(const sunspec::SunSpecThing& thing, const sunspec::StatsModel& model);

private:
    std::map<std::pair<std::string, uint16_t>, sunspec::StatsModel> m_models;
};
