#include "rtray.h"

RTRay::RTRay(RTPoint source, RTVector direction)
    : source(source), direction(direction)
{
    direction.normalize();
}

RTRay::RTRay(RTPoint &source, RTPoint &point)
    : source(source)
{
    float dx = point.x - source.x;
    float dy = point.y - source.y;
    float dz = point.z - source.z;
    direction = RTVector(dx, dy, dz);
    direction.normalize();
}
