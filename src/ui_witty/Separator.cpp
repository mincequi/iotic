#include "Separator.h"

#include <Wt/WPainter.h>

#include "theme/Colors.h"

using namespace Wt;

Separator::Separator() {
    setLayoutSizeAware(true);
    resize(WLength::Auto, 1.0);
}

void Separator::paintEvent(WPaintDevice* paintDevice) {
    WPainter painter(paintDevice);
    //painter.setShadow(WShadow(10, 10, WColor(0, 0, 0, 50), 10));
    painter.setRenderHint(RenderHint::Antialiasing, false);

    WPen pen;
    pen.setCapStyle(PenCapStyle::Flat);
    pen.setWidth(1.0);
    pen.setColor(Colors::white());
    painter.setPen(pen);
    painter.drawLine(0, 0, _width, 0.0);
}

void Separator::layoutSizeChanged(int width, int height) {
    _width = width;
    WPaintedWidget::layoutSizeChanged(width, height);
}
