#include "rtcanvasqt.h"

#include <QColor>
#include <QPainter>
#include <QThread>

RTCanvasQt::RTCanvasQt(QWidget *parent, int size) : QWidget(parent)
{
    setFixedSize(size, size);
    image = new QImage(this->size(), QImage::Format_ARGB32_Premultiplied);
    ready = false;
}

void RTCanvasQt::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(0, 0, *image);
}

void RTCanvasQt::setReady() { ready = true; }

void RTCanvasQt::fillBackground(int r, int g, int b) { image->fill(qRgb(r, g, b)); }
void RTCanvasQt::setPixel(uint x, uint y, int r, int g, int b) { image->setPixel(x, y, qRgb(r, g, b)); }
void RTCanvasQt::updateScreen() { this->repaint(); }

unsigned RTCanvasQt::getWidth() { return this->width(); }
unsigned RTCanvasQt::getHeight() { return this->height(); }
