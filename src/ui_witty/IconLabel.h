#pragma once

#include <Wt/WColor.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WFont.h>

class IconLabel : public Wt::WContainerWidget {
public:
    IconLabel(const std::string& icon = "",
              Wt::FontSize size = Wt::FontSize::XSmall,
              const Wt::WColor& color = Wt::StandardColor::White);

    void setValue(int value);
    void setUnit(const Wt::WString& unit);
    void setColor(const Wt::WColor& color);
    void setIcon(const std::string& faString);

    void setPower(int watts);
    void setProduction(int hundredWattHours);

private:
    Wt::FontSize _size = Wt::FontSize::XSmall;
    Wt::WIcon* _icon;
    Wt::WText* _value;
    Wt::WText* _unit;

    int _power = 0;
};
