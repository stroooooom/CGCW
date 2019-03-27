#include "rtcolor.h"

RTColor getMixedColor(RTColor c1, RTColor c2, float p)
{
    if (p >= 1) return c1;
    if (p <= 0) return c2;

    auto q = 1 - p;
    auto mixed_color = RTColor();

    mixed_color.r = c1.r * p + c2.r * q;
    mixed_color.g = c1.g * p + c2.g * q;
    mixed_color.b = c1.b * p + c2.b * q;

    return mixed_color;
}
