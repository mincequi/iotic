#pragma once

#include <ctime>

#include <rpp/observables.hpp>

#include <Wt/Chart/WAbstractChartModel.h>

class HistoryModel : public Wt::Chart::WAbstractChartModel {
public:
    using Series = std::vector<std::tuple<std::time_t, int, int>>;

    HistoryModel();

    // TODO: implement for real history mode
    void setTimespan(std::time_t from, std::time_t to);

    void addValue(std::time_t ts, int pvPower, int gridPower, int siteConsumption);

    /// Set stats data until we implement real history mode
    void setMaxValue(int value);

    double maxValue() const;

    Wt::WString headerData(int column) const override;
    double data(int row, int column) const override;
    int columnCount() const override;
    int rowCount() const override;

private:
    std::vector<std::tuple<std::time_t, int, int, int>> _data;

    double _maxValue = 0;
};
