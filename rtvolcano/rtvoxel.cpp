#include "rtvoxel.h"

RTVoxel::RTVoxel()
    : isEmpty(true), ambient_ratio(0), type(DEFAULT) {}

RTVoxel::RTVoxel(int r, int g, int b, float ambient_ratio)
    : isEmpty(false), ambient_ratio(ambient_ratio), type(DEFAULT) { this->setColor(r, g, b); }

RTVoxel::RTVoxel(RTColor color, float ambient_ratio)
    : isEmpty(false), ambient_ratio(ambient_ratio), type(DEFAULT) { this->color = color; }

RTVoxel::RTVoxel(RTVoxel &v)
{
    color = v.color;
    normal = v.normal;
    ambient_ratio = v.ambient_ratio;
    type = v.type;
    isEmpty = v.isEmpty;
}

RTVoxel::RTVoxel(RTVoxel &&v)
{
    color = v.color;
    normal = v.normal;
    ambient_ratio = v.ambient_ratio;
    type = v.type;
    isEmpty = false;
    v.isEmpty = true;
    v.type = DEFAULT;
}

void RTVoxel::operator=(const RTVoxel &v)
{
    color = v.color;
    normal = v.normal;
    ambient_ratio = v.ambient_ratio;
    type = v.type;
    isEmpty = v.isEmpty;
}

void RTVoxel::move(RTVoxel *v)
{
    color = v->color;
    normal = v->normal;
    ambient_ratio = v->ambient_ratio;
    type = v->type;
    isEmpty = false;
    v->isEmpty = true;
    v->type = DEFAULT;
}


void RTVoxel::setColor(int r, int g, int b)
{
    this->color.r = r;
    this->color.g = g;
    this->color.b = b;
}

void RTVoxel::setColor(RTColor &new_color)
{
    this->color = new_color;
}

RTVector RTVoxel::getNormalAtPoint2(uint x, uint y, uint z, RTVoxel ***grid, uint size)
{
    uint n = size - 1; // номер последней ячейки
    auto normal = ZERO;

    if (!(x != 0 && grid[x-1][y][z].isEmpty == false))  normal += LEFT;
    if (!(x != n && grid[x+1][y][z].isEmpty == false))  normal += RIGHT;
    if (!((y != 0 && grid[x][y-1][z].isEmpty == false) || (y == 0)))
                                                        normal += DOWN;
    if (!(y != n && grid[x][y+1][z].isEmpty == false))  normal += UP;
    if (!(z != 0 && grid[x][y][z-1].isEmpty == false))  normal += FRONT;
    if (!(z != n && grid[x][y][z+1].isEmpty == false))  normal += BACK;

    normal.normalize();

    return normal;
}

void RTVoxel::getNormalAtPoint(uint x, uint y, uint z, RTVoxel ***grid, uint size, RTVector *res)
{
    uint n = size - 1;
    auto normal = ZERO;

    if (!(x != 0 && grid[x-1][y][z].isEmpty == false))  normal += LEFT;
    if (!(x != n && grid[x+1][y][z].isEmpty == false))  normal += RIGHT;
    if (!((y != 0 && grid[x][y-1][z].isEmpty == false) || (y == 0)))
                                                        normal += DOWN;
    if (!(y != n && grid[x][y+1][z].isEmpty == false))  normal += UP;
    if (!(z != 0 && grid[x][y][z-1].isEmpty == false))  normal += FRONT;
    if (!(z != n && grid[x][y][z+1].isEmpty == false))  normal += BACK;

    normal.normalize();

    *res = (normal.getLength() < 1e-5f) ? UP : normal;
}
