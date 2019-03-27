#ifndef RTSCENE_H
#define RTSCENE_H

#include "rtvector.h"
#include "rtpoint.h"
#include "rtvoxel.h"
#include "rtflow.h"
#include <QColor>

class RTScene
{
    friend class RTFLow;

public:
    RTScene(uint size);
    ~RTScene();

    RTVoxel& operator()(uint x, uint y, uint z);

    void initVoxels();

    void calculateStaticVoxelsNormals();
    void calculateMovingVoxelsNormals();
    void _calculateMovingVoxelsNormals(uint id_start, uint id_end);

    void createMountain(float h = 0); // static model of mountain
    void moveLava();

    void reset();

    uint getSize();

private:
    void _circle(int z, int r, RTColor color);

public:
    uint size;
    RTVoxel ***grid;
    RTFlow *flow;

};

#endif // RTSCENE_H
