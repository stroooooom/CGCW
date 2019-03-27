#include "rtpoint.h"
#include <iostream>

RTPoint::RTPoint()
    : x(0), y(0), z(0) {}

RTPoint::RTPoint(float x, float y, float z)
    : x(x), y(y), z(z) {}

RTPoint& RTPoint::operator+=(const RTPoint &p)
{
    this->x += p.x;
    this->y += p.y;
    this->z += p.z;

    return *this;
}

RTPoint& RTPoint::operator=(const RTPoint &p)
{
    if (this != &p)
    {
        this->x = p.x;
        this->y = p.y;
        this->z = p.z;
    }

    return *this;
}

int RTPoint::getIntValue(float f) { return (f - int(f) >= 0.5f) ? f + 1 : f; }

void RTPoint::getIntValues(RTPoint &point, int &x, int &y, int &z)
{
    x = getIntValue(point.x);
    y = getIntValue(point.y);
    z = getIntValue(point.z);
}

RTPoint RTPoint::operator+(const RTPoint &p)
{
    RTPoint point(p);
    point.x += this->x;
    point.y += this->y;
    point.z += this->z;

    return point;
}

void RTPoint::debugPrint() { std::cout << this->x << " " << this->y << " " << this->z << std::endl; }
void RTPoint::debugPrint() const { std::cout << this->x << " " << this->y << " " << this->z << std::endl; }
