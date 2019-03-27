#ifndef RTRAY_H
#define RTRAY_H

#include "rtpoint.h"
#include "rtvector.h"

class RTRay
{
public:
    explicit RTRay(RTPoint source = RTPoint(), RTVector direction = RTVector());
    explicit RTRay(RTPoint &source, RTPoint &point);

    RTPoint source;
    RTVector direction;
};

#endif // RTRAY_H
