#ifndef RTVOXEL_H
#define RTVOXEL_H

#include "rtvector.h"
#include "rtcolor.h"

#define DEFAULT 0

typedef unsigned uint;

class RTVoxel
{
public:
    RTVoxel();
    RTVoxel(int r, int g, int b, float ambient_ratio = 0);
    RTVoxel(RTColor color, float ambient_ratio = 0);

    RTVoxel(RTVoxel &v);
    RTVoxel(RTVoxel &&v);

    void operator=(const RTVoxel &v);

    void move(RTVoxel *v);

    void setColor(int r, int g, int b);
    void setColor(RTColor &new_color);

    static RTVector getNormalAtPoint2(uint x, uint y, uint z, RTVoxel ***grid, uint size);
    static void getNormalAtPoint(uint x, uint y, uint z, RTVoxel ***grid, uint size, RTVector *res);

    bool isEmpty;
    RTColor color;
    RTVector normal;
    float ambient_ratio;

    int type;
};

#endif // RTVOXEL_H
