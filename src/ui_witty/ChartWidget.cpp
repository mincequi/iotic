#include "ChartWidget.h"

#include <Wt/WAbstractItemModel.h>
#include <Wt/WCanvasPaintDevice.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WDateTime.h>
#include <Wt/WPainter.h>
#include <Wt/WSvgImage.h>
#include <Wt/Chart/WAbstractChartModel.h>
#include <Wt/Chart/WAxis.h>

#include "theme/Colors.h"

const static int bottomPadding = 18;

using namespace Wt;

ChartWidget::ChartWidget(const std::shared_ptr<Wt::WAbstractItemModel>& model) {
    // Demo model setup
    model->setHeaderData(0, WString("X"));
    model->setHeaderData(1, WString("Y = sin(X)"));
    model->setHeaderData(2, WString("Y = cos(X)"));
    for (unsigned i = 0; i < 40; ++i) {
        double x = (static_cast<double>(i)) / 4;
        model->setData(i, 0, x);
        model->setData(i, 1, (1 + std::sin(x)) * 100);
        model->setData(i, 2, (1 + std::cos(x)) * 125);
    }
    setModel(model);
    init();
}

ChartWidget::ChartWidget(const std::shared_ptr<Wt::Chart::WAbstractChartModel>& model) {
    setModel(model);
    init();
}

void ChartWidget::setPvPower(int watts) {
    _pvPower = watts;
}

void ChartWidget::setMaxPvPower(int watts) {
    _maxPvPower = watts;
}

void ChartWidget::setConsumption(int watts) {
    _consumption = watts;
}

void ChartWidget::setMaxConsumption(int watts) {
    _maxConsumption = watts;
}

void ChartWidget::setExportPower(int watts) {
    _consumptionColor = (watts > 0) ? Colors::yellow() : Colors::orange();
}

void ChartWidget::init() {
    // Visual setup
    setBackground(Colors::background());
    resize(Wt::WLength::Auto, 194);
    //resize(288, 128);
    setLegendEnabled(true);
    setXSeriesColumn(0);
    setType(Wt::Chart::ChartType::Scatter);
    //setZoomEnabled();

    axis(Chart::Axis::X).setLocation(Chart::AxisValue::Zero);
    axis(Chart::Axis::Y).setLocation(Chart::AxisValue::Zero);

    setPlotAreaPadding(0, Side::Right);
    setPlotAreaPadding(-4, Side::Top);
    setPlotAreaPadding(bottomPadding, Side::Bottom);

    auto series = std::make_unique<Chart::WDataSeries>(1, Chart::SeriesType::Line);
    series->setPen(Colors::green());
    series->setFillRange(Chart::FillRangeType::ZeroValue);
    // TODO: set gradient here
    series->setBrush(Colors::green(97));
    addSeries(std::move(series));

    series = std::make_unique<Chart::WDataSeries>(2, Chart::SeriesType::Line);
    series->setPen(Colors::cyan());
    series->setFillRange(Chart::FillRangeType::ZeroValue);
    // TODO: set gradient here
    series->setBrush(Colors::cyan(97));
    addSeries(std::move(series));

    series = std::make_unique<Chart::WDataSeries>(3, Chart::SeriesType::Line);
    series->setPen(Colors::orange());
    series->setFillRange(Chart::FillRangeType::ZeroValue);
    // TODO: set gradient here
    series->setBrush(Colors::orange(97));
    addSeries(std::move(series));

    // setup axis
    //axis(Wt::Chart::Axis::X).setScale(Wt::Chart::AxisScale::DateTime);

    auto axisLabelFont = axis(Wt::Chart::Axis::X).labelFont();
    axisLabelFont.setSize(FontSize::XSmall);
    axis(Wt::Chart::Axis::X).setLabelFont(axisLabelFont);
    axis(Wt::Chart::Axis::X).setPen(Colors::white());
    axis(Wt::Chart::Axis::X).setTextPen(Colors::white());
    axis(Wt::Chart::Axis::X).setRange(0, 9);
    axis(Wt::Chart::Axis::X).setScale(Chart::AxisScale::DateTime);
    axis(Wt::Chart::Axis::Y).setLabelFont(axisLabelFont);
    axis(Wt::Chart::Axis::Y).setPen(Colors::white());
    axis(Wt::Chart::Axis::Y).setTextPen(Colors::white());
    axis(Wt::Chart::Axis::Y).setRange(0, 2500);
    //axis(Wt::Chart::Axis::X).setSoftLabelClipping(false);
}

void ChartWidget::setTimespan(std::time_t from, std::time_t to) {
    axis(Wt::Chart::Axis::X).setRange(from, to);
    axis(Wt::Chart::Axis::X).setLabelBasePoint(to-5);
}

void ChartWidget::setMaxValue(int value) {
    _maxValue = value;
    axis(Wt::Chart::Axis::Y).setRange(0, value);
}

void ChartWidget::layoutSizeChanged(int width, int height) {
    Wt::Chart::WCartesianChart::layoutSizeChanged(width, height);

    _width = width;
    _height = height - bottomPadding;
}

void ChartWidget::paintEvent(Wt::WPaintDevice *paintDevice) {
    Wt::Chart::WCartesianChart::paintEvent(paintDevice);

    // TODO: implement bar rendering once quality is alright
    return;
    static const double strength = 4.0;

    WPainter painter(paintDevice);
    //painter.setShadow(WShadow(10, 10, WColor(0, 0, 0, 50), 10));
    painter.setRenderHint(RenderHint::Antialiasing, false);

    WPen pen;
    pen.setCapStyle(PenCapStyle::Flat);
    pen.setWidth(1.0);
    pen.setColor(Colors::green());
    painter.setPen(pen);
    double height_ = _height * _maxPvPower / _maxValue;
    painter.drawLine(_width - 2 * strength - 2,
                     _height - height_,
                     _width - 2 * strength - 2,
                     _height);
    pen.setColor(Colors::orange());
    painter.setPen(pen);
    height_ = _height * _maxConsumption / _maxValue;
    painter.drawLine(_width - strength - 1,
                     _height - height_,
                     _width - strength - 1,
                     _height);

    pen.setWidth(strength);
    pen.setColor(Colors::green());
    painter.setPen(pen);
    painter.drawLine(_width - strength*1.5 - 2,
                     _height * (1 - _pvPower/_maxValue),
                     _width - strength*1.5 - 2,
                     _height);
    pen.setColor(Colors::orange());
    painter.setPen(pen);
    painter.drawLine(_width - strength*0.5 - 1,
                     _height * (1 - _consumption/_maxValue),
                     _width - strength*0.5 - 1,
                     _height);
}
