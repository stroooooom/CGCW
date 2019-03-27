#ifndef RTCAMERA_H
#define RTCAMERA_H

#include "rtpoint.h"


class RTCamera
{
public:
    RTCamera(RTPoint &pos, float vertical_axis_angle, float transverse_axis_angle, int distanceToCanvas);

    RTPoint viewPoint;
    float vertical_axis_angle;      // this axis has its origin at the viewPoint and is directed upward
    float transverse_axis_angle;    // this axis has its origin at the viewPoint and is directed to the right
    unsigned distanceToCanvas;      // needs to determine FOV
};

#endif // RTCAMERA_H
