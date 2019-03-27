#ifndef RTCANVASQT_H
#define RTCANVASQT_H

#include <QWidget>
#include <QObject>

#include <QImage>
#include "rtcanvas.h"

class RTCanvasQt : public QWidget, public RTCanvas
{
    Q_OBJECT
public:
    explicit RTCanvasQt(QWidget *parent = 0, int size = 100);

    void setReady();

    void fillBackground(int r, int g, int b);
    void setPixel(uint x, uint y, int r, int g, int b);
    void updateScreen();

    unsigned getWidth();
    unsigned getHeight();

protected:
    void paintEvent(QPaintEvent *);

private:
    QImage *image;
    bool ready;
};

#endif // RTCANVASQT_H
