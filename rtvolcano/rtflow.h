#ifndef RTFLOW_H
#define RTFLOW_H

#include "rtvoxel.h"
#include "rtvector.h"

#include <cstdlib>
#include <vector>

#include <vector>
#include <mutex>

#define DYNAMIC 1

class RTMovingVoxel
{
    friend class RTScene;
    friend class RTFlow;

public:
    RTMovingVoxel();

    RTPoint position; // position in the scene
    RTVector velocity;
    float t;

    bool stopped;

private:
    int x, y, z; // voxel position of this particle
};

//////
/// \class RTFlow
/// \brief RTFlow class controls moving voxels (particles) in the scene.
/// Class members:
/// source, direction - defines initial position and direction of particles movement;
/// speed_ratio - used in particles speed calculation, defines slowing of the particles movement;
/// magnitude - non-negative value which specifies number of particles spawned by spawnParticles();
/// t_default - specifies initial temperature of the each particle which reduces by t_delta each step.
class RTFlow // LAVA
{
    friend class RTScene;

public:
    RTFlow(RTVoxel ***cube, int size);

    RTPoint source;
    RTVector direction;
    uint magnitude;
    float speed_ratio;
    float t_default;
    float t_delta;

    void spawnParticles();
    void updateParticles();

    void resetFlow();

private:
    void _updateParticles(uint id_start, uint id_end);

    bool _createParticle(RTPoint position, RTVector velocity, float temrepature);
    void _moveParticle(RTMovingVoxel *mv);
    void _updateParticle(uint id);

    float __updateTemperature(int x, int y, int z);
    RTVector __getSpeed(RTMovingVoxel &mv, float p = 0);
    RTColor __getColor(float t);

    RTMovingVoxel *moving_voxels;
    uint moving_voxels_size;
    uint moving_voxels_capacity;

    RTVoxel ***cube;
    int size;
};

#endif // RTFLOW_H
