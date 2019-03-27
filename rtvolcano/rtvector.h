#ifndef RTVECTOR_H
#define RTVECTOR_H

#include "rtpoint.h"
#include <cmath>

class RTVector : public RTPoint
{
public:
    RTVector();
    explicit RTVector(float x, float y, float z, bool normilize = false);

    void normalize();
    float getLength() const;

    static float getAngle(const RTVector &v1, const RTVector &v2);
    static float dotProduct(const RTVector &v1, const RTVector &v2);
    static RTVector crossProduct(const RTVector &v1, const RTVector &v2);

    void operator*=(float n);
    void operator+=(RTVector &v1);
    RTVector operator*(float n);
    friend RTVector operator+(const RTVector &v1, const RTVector &v2);
    friend RTVector operator-(const RTVector &v1, const RTVector &v2);
    friend RTVector operator+(const RTVector &v, const RTPoint &p);
    friend RTVector operator-(const RTVector &v, const RTPoint &p);
    friend bool operator==(const RTVector &v1, const RTVector &v2);
};

static RTVector LEFT = RTVector(-1, 0, 0);
static RTVector RIGHT = RTVector(1, 0, 0);
static RTVector UP = RTVector(0, 1, 0);
static RTVector DOWN = RTVector(0, -1, 0);
static RTVector FRONT = RTVector(0, 0, 1);
static RTVector BACK = RTVector(0, 0, -1);
static RTVector ZERO = RTVector(0, 0, 0);

#endif // RTVECTOR_H
