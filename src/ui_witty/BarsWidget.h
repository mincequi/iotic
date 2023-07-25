#pragma once

#include <Wt/WPaintedWidget.h>

#include "theme/Colors.h"

class BarsWidget : public Wt::WPaintedWidget {
public:
    BarsWidget();

    void setPvPower(int watts);
    void setAvgPvPower(int watts);
    void setMaxPvPower(int watts);

    void setConsumption(int watts);
    void setExportPower(int watts);
    void setAvgExportPower(int watts);
    void setMaxPowerDemand(int watts);

protected:
    void paintEvent(Wt::WPaintDevice* paintDevice) override;

private:
    double _pvPower = 0;
    double _avgPvPower = 0;
    double _maxPvPower = 0;
    double _consumption = 0;
    Wt::WColor _consumptionColor = Colors::orange();
    double _avgExportPower = 0;
    double _maxPowerDemand = 0;
};
