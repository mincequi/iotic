#ifndef STATISTICS_H
#define STATISTICS_H

#include <QObject>

#include "sunspec/SunSpecModel.h"
#include "sunspec/SunSpecStatsModel.h"

namespace sunspec {
class SunSpecThing;
}

class Statistics : public QObject {
    Q_OBJECT
public:
    explicit Statistics(QObject *parent = nullptr);

    void feedModel(const sunspec::SunSpecThing& thing, const sunspec::Model& model);

signals:
    void statsChanged(const sunspec::SunSpecThing& thing, const sunspec::StatsModel& model);

private:
    std::map<std::pair<std::string, uint16_t>, sunspec::StatsModel> m_models;
};

#endif // STATISTICS_H
