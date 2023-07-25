#pragma once

#include <Wt/WPaintedWidget.h>

namespace Wt {
class WPainter;
}

class ArrowWidget : public Wt::WPaintedWidget {
public:
    ArrowWidget();

    void setPower(int watts);

protected:
    void paintEvent(Wt::WPaintDevice *paintDevice) override;

private:
    int _power = 0;

    void drawEmwebE(Wt::WPainter& p);
    void drawEmwebMW(Wt::WPainter& p);
};
