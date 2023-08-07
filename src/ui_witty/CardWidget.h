#pragma once

#include <Wt/WContainerWidget.h>

class IconLabel;

class CardWidget : public Wt::WContainerWidget {
public:
    CardWidget();

    void setPhases(int phases);
    void setPower(int watts);

    Wt::Signal<int>& allowedGridPowerChanged();

private:
    IconLabel* _currentPowerLabel;
    Wt::WSlider* _slider;
    IconLabel* _allowedGridPowerLabel;

    Wt::Signal<int> _allowedGridPowerChanged;
};
