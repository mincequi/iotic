#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <Wt/Chart/WCartesianChart.h>

#include "theme/Colors.h"

namespace Wt {
class WAbstractItemModel;
namespace Chart {
class WAbstractChartModel;
}
}

class ChartWidget : public Wt::Chart::WCartesianChart {
public:
    ChartWidget(const std::shared_ptr<Wt::WAbstractItemModel>& model);
    ChartWidget(const std::shared_ptr<Wt::Chart::WAbstractChartModel>& model);

    void setPvPower(int watts);
    void setMaxPvPower(int watts);

    void setConsumption(int watts);
    void setMaxConsumption(int watts);

    void setExportPower(int watts);

    void setTimespan(std::time_t from, std::time_t to);
    void setMaxValue(int value);

private:
    void init();

    void layoutSizeChanged(int width, int height) override;
    void paintEvent(Wt::WPaintDevice *paintDevice) override;

    int _width = 0;
    int _height = 0;
    double _maxValue = 1;

    double _pvPower = 0;
    double _maxPvPower = 0;

    double _consumption = 0;
    double _maxConsumption = 0;
    Wt::WColor _consumptionColor = Colors::orange();
};

#endif // CHARTWIDGET_H
