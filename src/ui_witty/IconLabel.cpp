#include "IconLabel.h"

#include <cmath>

#include <Wt/WCssDecorationStyle.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WIcon.h>
#include <Wt/WServer.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>

#include "ArrowWidget.h"
#include "theme/Colors.h"
#include "util/Formatters.h"

using namespace Wt;

IconLabel::IconLabel(const std::string& icon,
                     FontSize size,
                     const WColor& color) :
    _size(size) {
    resize(size < FontSize::Medium ? 56.0 : 64, Wt::WLength::Auto);
    auto hbox = setLayout(std::make_unique<Wt::WHBoxLayout>());
    hbox->setContentsMargins(0, 0, 0, 0);
    hbox->setSpacing(0);
    _icon = hbox->addWidget(std::make_unique<Wt::WIcon>("fa " + icon), 1, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle);

    _value = hbox->addWidget(std::make_unique<Wt::WText>(""), 0, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);
    _value->decorationStyle().font().setSize(_size);
    _value->decorationStyle().font().setWeight(FontWeight::Bolder);
    hbox->addSpacing(size < FontSize::Medium ? 3 : 6);

    _unit = hbox->addWidget(std::make_unique<Wt::WText>("kW"), 0, AlignmentFlag::Right | AlignmentFlag::Middle);
    _unit->decorationStyle().font().setSize(_size);

    setColor(color);
}

void IconLabel::setValue(int value) {
    _value->setText(std::to_string(value));
}

void IconLabel::setUnit(const Wt::WString& unit) {
    _unit->setText(unit);
}

void IconLabel::setColor(const Wt::WColor& color) {
    _icon->decorationStyle().setForegroundColor(color);
    _value->decorationStyle().setForegroundColor(color);
    _unit->decorationStyle().setForegroundColor(Colors::withLightness(color, 0.68));
}

void IconLabel::setIcon(const std::string& faString) {
    _icon->setName("fa " + faString);
}

void IconLabel::setPower(int watts) {
    if (watts == std::numeric_limits<int>::min()) {
        _value->setText("Off");
        _unit->setText("");
        return;
    }

    _power = watts;

    auto [value, unit] = fmt::toPower(watts);
    _value->setText(value);
    _unit->setText(unit);

    Wt::WServer::instance()->postAll([this]() {
        wApp->triggerUpdate();
    });
}

void IconLabel::setProduction(int hundredWattHours) {

}
