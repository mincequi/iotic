#include "BarsWidget.h"

#include <Wt/WPainter.h>

#include "theme/Colors.h"

static const double strength = 3.0;

using namespace Wt;

BarsWidget::BarsWidget() {
    resize(strength*2 + 1, 194.0 - 18.0);
}

void BarsWidget::setPvPower(int watts) {
    _pvPower = watts;
}

void BarsWidget::setAvgPvPower(int watts) {
    _avgPvPower = watts;
}

void BarsWidget::setMaxPvPower(int watts) {
    _maxPvPower = watts;
}

void BarsWidget::setConsumption(int watts) {
    _consumption = watts;
}

void BarsWidget::setExportPower(int watts) {
    _consumptionColor = (watts > 0) ? Colors::yellow() : Colors::orange();
}

void BarsWidget::setAvgExportPower(int watts) {
    _avgExportPower = watts;
}

void BarsWidget::setMaxPowerDemand(int watts) {
    _maxPowerDemand = watts;
}

void BarsWidget::paintEvent(WPaintDevice* paintDevice) {
    const auto maxValue = std::max(_maxPvPower, _maxPowerDemand);

    WPainter painter(paintDevice);
    //painter.setShadow(WShadow(10, 10, WColor(0, 0, 0, 50), 10));
    painter.setRenderHint(RenderHint::Antialiasing, false);

    WPen pen;
    pen.setCapStyle(PenCapStyle::Flat);
    pen.setWidth(1.0);
    pen.setColor(Colors::withAlpha(Colors::green(), 0.6));
    painter.setPen(pen);
    double height_ = height().value() * _maxPvPower / maxValue;
    painter.drawLine(0.5, height().value() - height_, 0.5, height().value());
    pen.setColor(Colors::withAlpha(_consumptionColor, 0.6));
    painter.setPen(pen);
    height_ = height().value() * _maxPowerDemand / maxValue;
    painter.drawLine(strength + 1.5, height().value() - height_, strength + 1.5, height().value());

    pen.setWidth(strength);
    pen.setColor(Colors::green());
    painter.setPen(pen);
    painter.drawLine(strength/2, height().value() * (1 - _pvPower/maxValue), strength/2, height().value());
    pen.setColor(_consumptionColor);
    painter.setPen(pen);
    painter.drawLine(strength*1.5 + 1, height().value() * (1 - _consumption/maxValue), strength*1.5 + 1, height().value());
}
