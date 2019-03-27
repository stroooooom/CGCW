#include "rtcamera.h"

RTCamera::RTCamera(RTPoint &pos, float vertical_axis_angle, float transverse_axis_angle, int distanceToCanvas)
    : viewPoint(pos),
      vertical_axis_angle(vertical_axis_angle),
      transverse_axis_angle(transverse_axis_angle),
      distanceToCanvas(distanceToCanvas) {}
