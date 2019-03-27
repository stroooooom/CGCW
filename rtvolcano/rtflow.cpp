#include "rtflow.h"
#include "rtpoint.h"
#include <cmath>

#include <iostream>
#include <thread>

#define MAX_PARTICLES_SPAWNED 1500
#define DEFAULT_CAPACITY 2500

#define THREADCOUNT 24

#define TMPRT_STBL 0.9981f
#define TMRPT_MV_LMT 6e-3f
#define TMPRT_EPS 1e-3f

/* MOVING VOXEL */
RTMovingVoxel::RTMovingVoxel()
    :position(ZERO), velocity(ZERO), t(0), stopped(false) {}

/* FLOW */
/// \brief Constructs RTflow object using \a cube pointing to scene voxels array, scene \a size
///  and maximum number of particles equal DEFAULT_CAPACITY
RTFlow::RTFlow(RTVoxel ***cube, int size)
    :moving_voxels_capacity(DEFAULT_CAPACITY), cube(cube), size(size)
{
    moving_voxels = new RTMovingVoxel[DEFAULT_CAPACITY];
    moving_voxels_size = 0;
}

void RTFlow::spawnParticles()
{
    int src_x, src_y, src_z;
    RTPoint::getIntValues(source, src_x, src_y, src_z);

    auto velocity = direction * speed_ratio;

    int l = -((int)magnitude)/2;
    int r = -l + 1;

    for (int dx = l; dx < r; dx++)
        for (int dy = l; dy < r; dy++)
            for (int dz = l; dz < r; dz++)
                _createParticle(RTPoint(src_x + dx, src_y + dy, src_z + dz), velocity, t_default);
}

void RTFlow::_updateParticles(uint id_start, uint id_end)
{
    for (uint i = id_start; i < id_end; i++)
        _updateParticle(i);
}

void RTFlow::updateParticles()
{
    for (uint i = 0; i < moving_voxels_size; i++)
        _moveParticle(moving_voxels+i);

    std::vector<std::thread> thread_pool;
    uint n = (moving_voxels_size/THREADCOUNT)+1;
    uint start = 0;
    uint end = n;

    while (start < moving_voxels_size)
    {
        thread_pool.push_back(
                    std::thread(&RTFlow::_updateParticles, this, start, end));
        start = end;
        end += n;
        if (end > moving_voxels_size)
            end = moving_voxels_size;
    }

    for (uint i = 0; i < thread_pool.size(); i++)
        thread_pool[i].join();
}

bool RTFlow::_createParticle(RTPoint position, RTVector velocity, float temrepature) // функция создает инстанс частицы (движ вокселя), добавляя его в список
{
    if (moving_voxels_size >= moving_voxels_capacity)
        return false;

    int idx, idy, idz;
    RTPoint::getIntValues(position, idx, idy, idz);

    if (idx < 0 || idx >= size || idy < 0 || idy >= size || idz < 0 || idz >= size)
        return false;

    if (cube[idx][idy][idz].isEmpty == false)
        return false;

    auto new_mv = RTMovingVoxel{};
    new_mv.position = position;
    new_mv.velocity = velocity;
    new_mv.t = temrepature;
    new_mv.x = idx;
    new_mv.y = idy;
    new_mv.z = idz;

    cube[idx][idy][idz].ambient_ratio = 0.7f;
    cube[idx][idy][idz].type = DYNAMIC;
    cube[idx][idy][idz].isEmpty = false;
    cube[idx][idy][idz].color = YELLOW;

    moving_voxels[moving_voxels_size++] = new_mv;

    return true;
}

void RTFlow::_moveParticle(RTMovingVoxel *mv)
{
    if (mv->stopped)
        return;

    int init_x = mv->x, init_y = mv->y, init_z = mv->z;

    //// частица лавы движется вниз, если под ней ничего нет
    if (mv->y > 1 && cube[init_x][init_y-1][init_z].isEmpty)
    {
        mv->position.y -= 1;
        mv->y -= 1;

        cube[mv->x][mv->y][mv->z].move(cube[init_x][init_y]+init_z); // переносим воксель в новую клетку
        return;
    }

    int x = mv->x, z = mv->z;

    auto cur_velocity = __getSpeed(*mv);
    float vx = cur_velocity.x;
    float vz = cur_velocity.z;

    int new_x = RTPoint::getIntValue(mv->position.x + vx);
    int new_z = RTPoint::getIntValue(mv->position.z + vz);

    if (new_x < 0 || new_x >= size || new_z < 0 || new_z >= size)
        return;

    if (init_x == new_x && init_z == new_z) // видимого сдвига нет
    {
        mv->position.x += mv->velocity.x;
        mv->position.z += mv->velocity.z;

        return;
    }

    bool xFirst = (fabs(vx) >= fabs(vz)) ? true : false;
    if (xFirst)
    {
        if (cube[new_x][init_y][init_z].isEmpty)
        {
            mv->position.x += mv->velocity.x;
            x = new_x;
        }

        if (cube[x][init_y][new_z].isEmpty)
        {
            mv->position.z += mv->velocity.z;
            z = new_z;
        }
    }
    else
    {
        if (cube[init_x][init_y][new_z].isEmpty)
        {
            mv->position.z += mv->velocity.z;
            z = new_z;
        }

        if (cube[new_x][init_y][z].isEmpty)
        {
            mv->position.x += mv->velocity.x;
            x = new_x;
        }
    }

    if (x != init_x || z != init_z) // найдена новая позиция
    {
        mv->x = x;
        mv->z = z;

        cube[x][init_y][z].move(cube[init_x][init_y]+init_z); // переносим воксель в новую клетку
    }
}

void RTFlow::_updateParticle(uint id)
{
    auto mv = moving_voxels+id;
    if (mv->t < TMPRT_EPS)
        return;

    auto t = __updateTemperature(mv->x, mv->y, mv->z);

    mv->t = mv->t * TMPRT_STBL +  t * (1 - TMPRT_STBL);
    mv->t -= t_delta;

    cube[mv->x][mv->y][mv->z].color = __getColor(mv->t);

    if (mv->t < TMRPT_MV_LMT)
    {
        mv->stopped = true;
        cube[mv->x][mv->y][mv->z].type = DEFAULT;
    }
}

float RTFlow::__updateTemperature(int x, int y, int z)
{
    auto adjacent_mv = 0;
    float common_t = 0;
    for (int dx = -1; dx < 2; dx++)
        for (int dy = -1; dy < 2; dy++)
            for (int dz = -1; dz < 2; dz++)
            {
                int cur_x = x + dx;
                int cur_y = y + dy;
                int cur_z = z + dz;

                if (cur_x < 0 || cur_x >= size
                 || cur_y < 0 || cur_y >= size
                 || cur_z < 0 || cur_z >= size)
                    continue;

                auto cur_cell = cube[cur_x][cur_y][cur_z];
                if (!cur_cell.isEmpty && cur_cell.type == DYNAMIC)
                {
                    for (int i = 0; i < moving_voxels_size; i++)
                    {
                        auto cur_mv = moving_voxels[i];
                        if (cur_mv.x == cur_x && cur_mv.y == cur_y && cur_mv.z == cur_z)
                        {
                            adjacent_mv++;
                            common_t += cur_mv.t;
                        }
                    }
                }
            }

    return common_t/27;
}

RTColor RTFlow::__getColor(float t)
{
    auto threshold_1 = t_default*0.65f;
    auto threshold_2 = t_default*0.1f;

    float p;
    if (t > threshold_1)
    {
        p = (t - threshold_1)/(1 - threshold_1);
       return getMixedColor(YELLOW, RED, p);
    }
    else if (t > threshold_2)
    {
        p = (t - threshold_2)/(1 - threshold_2);
        return getMixedColor(RED, DARKRED, p);
    }
    else
    {
        p = t/threshold_2;
        return getMixedColor(DARKRED, GRAY, p);
    }
}

RTVector RTFlow::__getSpeed(RTMovingVoxel &mv, float p)
{
    auto t = mv.t;

    float l = 1;
    int k;
    if (t > t_default*0.6)
    {
       k = 2;
    }
    else if (t > t_default*0.15)
    {
        k = 4;
    }
    else
    {
        k = 5;
        l = 0.5f;
    }

    return mv.velocity * pow((t/t_default), k) * speed_ratio * l;
}

void RTFlow::resetFlow()
{
    moving_voxels_size = 0;
}
