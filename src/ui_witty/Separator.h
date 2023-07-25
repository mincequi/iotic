#ifndef SEPARATOR_H
#define SEPARATOR_H

#include <Wt/WPaintedWidget.h>

class Separator : public Wt::WPaintedWidget {
public:
    Separator();

protected:
    void paintEvent(Wt::WPaintDevice* paintDevice) override;
    void layoutSizeChanged(int width, int height) override;

private:
    int _width = 0;
};

#endif // SEPARATOR_H
