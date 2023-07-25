#include "CardWidget.h"

#include <array>
#include <cmath>

#include <Wt/WAnimation.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WFont.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WIcon.h>
#include <Wt/WLabel.h>
#include <Wt/WPanel.h>
#include <Wt/WSlider.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>

#include "IconLabel.h"
#include "Separator.h"
#include "theme/Colors.h"
#include "util/Formatters.h"

using namespace Wt;

static const std::array<int, 9> values = { std::numeric_limits<int>::min(), -1000, -500, 0, 500, 1000, 2000, 5000, 10000 };

CardWidget::CardWidget() {
    setStyleClass("card bg-dark");
    resize(Wt::WLength::Auto, Wt::WLength::Auto);

    // Main Layout
    auto mainLayout = setLayout(std::make_unique<Wt::WVBoxLayout>());
    mainLayout->setContentsMargins(16, 0, 16, 0);
    mainLayout->setSpacing(0);

    // Header
    auto headerLayout = mainLayout->addLayout(std::make_unique<Wt::WHBoxLayout>());
    // TODO: these values are not respected by icon and title. Why?
    headerLayout->setContentsMargins(16, 0, 16, 0);
    headerLayout->setSpacing(0);
    auto icon = headerLayout->addWidget(std::make_unique<Wt::WIcon>("fa fa-lg fa-car"), 0, AlignmentFlag::Middle);
    // We have to set this here, since headerLayout margins are not rspected.
    icon->setMargin(16, Side::Right | Side::Top | Side::Bottom);
    auto title = headerLayout->addWidget(std::make_unique<Wt::WText>("Go-eCharger-072217"), 1, AlignmentFlag::Left | AlignmentFlag::Middle);
    title->decorationStyle().font().setSize(Wt::FontSize::Large);
    _currentPowerLabel = headerLayout->addWidget(std::make_unique<IconLabel>("", FontSize::Medium), 0, AlignmentFlag::Right);
    _currentPowerLabel->setPower(0);
    mainLayout->addWidget(std::make_unique<Separator>());
    mainLayout->addSpacing(16);

    // Content
    auto contentLayout = mainLayout->addLayout(std::make_unique<Wt::WHBoxLayout>());
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(std::make_unique<Wt::WText>("Allowed grid power"));
    _allowedGridPowerLabel = contentLayout->addWidget(std::make_unique<IconLabel>("", FontSize::Medium), 0, AlignmentFlag::Right);
    //_allowedGridPowerValue = contentLayout->addWidget(std::make_unique<Wt::WText>(), 0, AlignmentFlag::Right);
    _slider = mainLayout->addWidget(std::make_unique<WSlider>());
    _slider->setRange(0, 8); // Off, -1000 -500, 0, 500, 1000, 2000, 5000, 10000
    _slider->sliderMoved().connect([=](int val) {
        _allowedGridPowerLabel->setPower(values.at(val));
    });
    _slider->valueChanged().connect([=](int val) {
        _allowedGridPowerChanged.emit(values.at(val));
    });
    _slider->setValue(3);


    mainLayout->addWidget(std::make_unique<WContainerWidget>(), 1);
    //contentLayout->
}

void CardWidget::setPower(int watts) {
    _currentPowerLabel->setPower(watts);
}

void CardWidget::setPhases(int phases) {

}

Wt::Signal<int>& CardWidget::allowedGridPowerChanged() {
    return _allowedGridPowerChanged;
}
