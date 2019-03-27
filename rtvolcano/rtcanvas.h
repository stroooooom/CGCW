#ifndef RTCANVAS_H
#define RTCANVAS_H

class RTCanvas
{
public:
    virtual void fillBackground(int r, int g, int b) = 0; // ?

    virtual void setPixel(unsigned x, unsigned y, int r, int g, int b) = 0;
    virtual void updateScreen() = 0;

    virtual unsigned getWidth() = 0;
    virtual unsigned getHeight() = 0;
};

#endif // RTCANVAS_H
