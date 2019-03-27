#include "rtscene.h"

#include <vector>
#include <thread>

#define THREADCOUNT 20
#define DEFAULT_SPEED_RATIO 0.4f

static bool s = false;

RTScene::RTScene(uint size)
    : size(size)
{
    grid = new RTVoxel** [size];
    for (uint i = 0; i < size; i++)
    {
        grid[i] = new RTVoxel* [size];
        for (uint j = 0; j < size; j++)
            grid[i][j] = new RTVoxel [size];
    }

    flow = new RTFlow(grid, size);
    flow->direction = RTVector(-0.75, -1, -1);
    flow->speed_ratio = DEFAULT_SPEED_RATIO;

    this->initVoxels();
}


void RTScene::initVoxels()
{
    RTVoxel voxel;
    for (uint x = 0; x < size; x++)
        for (uint y = 0; y < size; y++)
            for (uint z = 0; z < size; z++)
                grid[x][y][z] = voxel;
}

void RTScene::calculateStaticVoxelsNormals()
{
    for (uint x = 0; x < size; x++)
        for (uint y = 0; y < size; y++)
            for (uint z = 0; z < size; z++)
                RTVoxel::getNormalAtPoint(x, y, z, grid, size, &(grid[x][y][z].normal));
}

void RTScene::_calculateMovingVoxelsNormals(uint id_start, uint id_end)
{
    for (uint i = id_start; i < id_end; i++)
    {
        auto mv = flow->moving_voxels[i];
        RTVoxel::getNormalAtPoint(mv.x, mv.y, mv.z, grid, size, &(grid[mv.x][mv.y][mv.z].normal));
    }
}

void RTScene::calculateMovingVoxelsNormals()
{
    std::vector<std::thread> thread_pool;
    uint n = (flow->moving_voxels_size/THREADCOUNT)+1;
    uint start = 0;
    uint end = n;

    while (end < flow->moving_voxels_size)
    {
        thread_pool.push_back(
                    std::thread(&RTScene::_calculateMovingVoxelsNormals, this, start, end));
        start = end;
        end += n;
        if (end > flow->moving_voxels_size)
            end = flow->moving_voxels_size;
    }

    for (uint i = 0; i < thread_pool.size(); i++)
        thread_pool[i].join();
}

void RTScene::createMountain(float h)
{
    int r_max = this->size * 0.375;

    for (int r = 0; r < r_max; r++)
        _circle(0, r, GRAY);

    for (int z = 0; z < h; z++)
    {
        for (int ri = 0; ri <= r_max - z; ri++)
            _circle(z, ri, GRAY);

        for (int ri = 0; ri < r_max - h; ri++)
            _circle(z, ri, getMixedColor(YELLOW, RED, 0.2f));
    }

    auto v = RTVoxel(SANDY);
    for (uint x = 0; x < this->size; x++)
        for (uint z = 0; z < this->size; z++)
            this->grid[x][0][z] = v;
}

void RTScene::moveLava()
{
    flow->spawnParticles();
    flow->updateParticles();
    calculateMovingVoxelsNormals();
}

void RTScene::reset()
{
    for (uint i = 0; i < flow->moving_voxels_size; i++)
    {
        auto mv = flow->moving_voxels[i];
        grid[mv.x][mv.y][mv.z].isEmpty = true;
    }

    flow->moving_voxels_size = 0;
}

void RTScene::_circle(int z, int r, RTColor color)
{
    auto voxel = RTVoxel(color);

    int xc = this->size / 2;
    int yc = xc;

    int x = 0;
    int y = r;
    int delta = 2 - 2*r;
    int error;
    while (y >= 0)
    {
        grid[xc+x][z][yc+y] = voxel;
        grid[xc+x][z][yc-y] = voxel;
        grid[xc-x][z][yc+y] = voxel;
        grid[xc-x][z][yc-y] = voxel;

        error = 2 * (delta + y) - 1;
        if (delta < 0 && error <= 0)
        {
            ++x;
            delta += 2*x + 1;
            continue;
        }

        if (delta > 0 && error > 0)
        {
            --y;
            delta += 1 - 2*y;
            continue;
        }

        ++x;
        delta += 2 * (x-y);
        --y;
    }

}

RTVoxel& RTScene::operator()(uint x, uint y, uint z) { return grid[x][y][z]; }

uint RTScene::getSize() { return this->size; }

RTScene::~RTScene()
{
    for (uint i = 0; i < size; i++)
    {
        for (uint j = 0; j < size; j++)
        {
            delete [] grid[i][j];
        }
        delete [] grid[i];
    }
    delete [] grid;
}
