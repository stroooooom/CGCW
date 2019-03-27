#include "rtrender.h"
#include "rtcolor.h"
#include <cmath>
#include <vector>
#include <thread>

#define PI 3.1416
#define RAY_DEPTH 805

#include <QTime>
#include <QColor>

#define THREADSCOUNT 24

RTRender::RTRender(RTScene *cube, RTCanvas *canvas)
    :scene(cube), canvas(canvas)
{
    lightOn = true;
    state = READY;
    mode = REALTIME;

    this->backgroundColor = PURPLE;

    auto gridSize = scene->getSize();
    auto viewPoint = RTPoint(gridSize / 10, gridSize/2, -0.6f*gridSize);
    camera = new RTCamera(viewPoint, 0, 0, gridSize);
    camera->vertical_axis_angle = -20;
    camera->transverse_axis_angle = -40;

    lightSource = RTVector(gridSize / 2, gridSize, gridSize / 2);
    light_coeff = 1;

    _canvas_width = canvas->getWidth();
    _canvas_height = canvas->getHeight();
    _canvas_width_half = _canvas_width / 2;
    _canvas_height_half = _canvas_height / 2;
    _rays = nullptr;

    frameBuffer = new RTColor* [_canvas_width];
    for (uint i = 0; i < _canvas_width; i++)
        frameBuffer[i] = new RTColor [_canvas_height];

    frames = nullptr;
    frames_number = 0;
    frames_rendered = 0;
    mode = REALTIME;
    state = READY;

    scene->calculateStaticVoxelsNormals();

    _precomputeValues();
}

void RTRender::paint(int frame_number)
{
    RTColor **frame;
    if (mode == REALTIME) frame = frameBuffer;
    else if (mode == PRERENDER)
    {
        if (frame_number < 0 || frame_number >= frames_rendered) return;
        frame = frames[frame_number];
    }

    RTColor rgb;
    for (uint j = 0; j < _canvas_height; j++)
    {
        for (uint i = 0; i < _canvas_width; i++)
        {
            rgb = frame[i][j];
            canvas->setPixel(i, j, rgb.r, rgb.g, rgb.b);
        }
    }

    canvas->updateScreen();
}

void RTRender::_renderFrame(uint id_start, uint id_end, RTColor **frame)
{
    int r, g, b;
    for (uint j = id_start; j < id_end; j++)
    {
        for (uint i = 0; i < _canvas_width; i++)
        {
            RTPoint hitPoint;
            bool hit = traceRay_3DDDA(_rays[i][j], r, g, b, hitPoint);

            if (hit == false)
            {
                frame[i][j] = backgroundColor;
                continue;
            }

            int x, y, z;
            RTPoint::getIntValues(hitPoint, x, y, z);

            RTColor materialColor = {r, g, b};
            if (lightOn)
            {
                auto ambient_ratio = scene->grid[x][y][z].ambient_ratio;
                auto n = scene->grid[x][y][z].normal;
                auto l = lightSource - hitPoint;

                auto angle = RTVector::getAngle(l, n);
                frame[i][j] = light(materialColor, angle, ambient_ratio);
            }
            else
            {
                frame[i][j] = materialColor;
            }
        }
    }
}

void RTRender::renderFrame()
{
    std::vector<std::thread> thread_pool;

    RTColor **frame;
    if (mode == REALTIME) frame = this->frameBuffer;
    else if (mode == PRERENDER)
    {
        if (frames_rendered < frames_number)
        {
            frame = frames[frames_rendered];
            frames_rendered += 1;
        }
        else
            return;
    }

    auto lines_per_thread = ((int) _canvas_height/THREADSCOUNT) + 1;
    uint id_start = 0;
    uint id_end = lines_per_thread;

    for (int i = 0; i < THREADSCOUNT; i++)
    {
        if (id_end > _canvas_height) id_end = _canvas_height;

        thread_pool.push_back(std::thread(&RTRender::_renderFrame, this, id_start, id_end, frame));

        id_start += lines_per_thread;
        id_end += lines_per_thread;
    }

    for (int i = 0; i < THREADSCOUNT; i++) thread_pool[i].join();
}

void RTRender::resetScene() { scene->reset(); }

RTColor RTRender::light(const RTColor &color, float angle, float ambient_k)
{
    float cos_alpha = cos(angle) < 0 ? 0 : cos(angle);
    float ratio = ambient_k;
    ratio += cos_alpha*light_coeff;
    ratio = (ratio > 1) ? 1 : ratio;

    int r = (float) color.r * ratio;
    int g = (float) color.g * ratio;
    int b = (float) color.b * ratio;

    return RTColor{r, g, b};
}

bool RTRender::traceRay_3DDDA(RTRay &ray, int &r, int &g, int &b, RTPoint &point)
{
    int X, Y, Z;
    RTPoint::getIntValues(ray.source, X, Y, Z);

    int stepX = ray.direction.x > 0 ? 1 : -1;
    int stepY = ray.direction.y > 0 ? 1 : -1;
    int stepZ = ray.direction.z > 0 ? 1 : -1;

    float tDeltaX = stepX/ray.direction.x;
    float tDeltaY = stepY/ray.direction.y;
    float tDeltaZ = stepZ/ray.direction.z;

    float tMaxX = ray.direction.x != 0 ? tDeltaX : INFINITY;
    float tMaxY = ray.direction.y != 0 ? tDeltaY : INFINITY;
    float tMaxZ = ray.direction.z != 0 ? tDeltaZ : INFINITY;

    auto gridSize = scene->getSize();

    int d = 0;
    while (d < RAY_DEPTH)
    {
        if (X >= 0 && X < gridSize && Y >= 0 && Y < gridSize && Z >= 0 && Z < gridSize)
        {
            RTVoxel voxel = (*scene)(X, Y, Z);
            if (voxel.isEmpty == false)
            {
                r = voxel.color.r;
                g = voxel.color.g;
                b = voxel.color.b;

                point = RTPoint(X, Y, Z);

                return true;
            }
        }

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                X += stepX;
                tMaxX += tDeltaX;
            } else {
                Z += stepZ;
                tMaxZ += tDeltaZ;
            }
        } else {
            if (tMaxY < tMaxZ) {
                Y += stepY;
                tMaxY += tDeltaY;
            } else {
                Z += stepZ;
                tMaxZ += tDeltaZ;
            }
        }

        if (Z >= gridSize) { return false; }

        d += 1;
    }

    return false;
}

RTPoint RTRender::canvasCoordToWorldPoint(int x, int y)
{
    auto viewPoint = camera->viewPoint;
    float wx = viewPoint.x + x * _vratio - _canvas_width_half;
    float wy = viewPoint.y + y * _hratio;
    float wz = viewPoint.z + camera->distanceToCanvas;

    wx -= viewPoint.x; // перенос точки в
    wz -= viewPoint.z; // центр координат

    // поворот вокруг оси X
    float wy_tmp2 = wy * _cos_vangle + wz * _sin_vangle;
    float wz_tmp2 = -wy * _sin_vangle + wz * _cos_vangle;

    wy = wy_tmp2;
    wz = wz_tmp2;

    // поворот вокруг оси Y (направлена вверх)
    float wx_tmp1 = wx * _cos_hangle - wz * _sin_hangle;
    float wz_tmp1 = wx * _sin_hangle + wz * _cos_hangle;

    wx = wx_tmp1;
    wz = wz_tmp1;

    wx += viewPoint.x; // обратный
    wz += viewPoint.z; // перенос

    return RTPoint(wx, wy, wz);
}

void RTRender::_precomputeValues()
{
    auto gridSize = scene->getSize();

    float ratio = PI / 180;
    float vangle_rad = camera->transverse_axis_angle * ratio;
    float hangle_rad = camera->vertical_axis_angle * ratio;

    _hratio = gridSize/_canvas_width;
    _vratio = gridSize/_canvas_height;


    _cos_vangle = cos(vangle_rad);
    _sin_vangle = sin(vangle_rad);

    _cos_hangle = cos(hangle_rad);
    _sin_hangle = sin(hangle_rad);

    if (_rays == nullptr)
    {
        _rays = new RTRay*[_canvas_height];
        for (uint i = 0; i < _canvas_height; i++)
            _rays[i] = new RTRay[_canvas_width];
    }

    for (uint j = 0; j < _canvas_height; j++)
    {
        for (uint i = 0; i < _canvas_width; i++)
        {
            auto projectionPoint = canvasCoordToWorldPoint(i, _canvas_height - j); // можно инвертирование по y занести в саму ф-цию
            auto ray = RTRay(camera->viewPoint, projectionPoint);
            while (ray.source.z < 0)
            {
                ray.source.x += ray.direction.x;
                ray.source.y += ray.direction.y;
                ray.source.z += ray.direction.z;
            }
            _rays[i][j] = ray;
        }
    }
}

void RTRender::removeFrame(RTColor **frame)
{
    if (frame == nullptr) return;

    for (uint i = 0; i < _canvas_width; i++)
        delete frame[i];

    delete frame;
}

void RTRender::removeFrames(RTColor ***frames)
{
    if (frames == nullptr) return;

    for (uint k = 0; k < frames_number; k++)
        removeFrame(frames[k]);

    delete frames;
}

void RTRender::createFrames(uint n)
{
    if (frames != nullptr)
        removeFrames(frames);

    frames = new RTColor** [n];
    for (uint k = 0; k < n; k++)
    {
        frameBuffer = new RTColor* [_canvas_width];
        for (uint i = 0; i < _canvas_width; i++)
            frameBuffer[i] = new RTColor [_canvas_height];
        frames[k] = frameBuffer;
    }

    frames_number = n;
    frames_rendered = 0;
}

void RTRender::changeMode(Mode new_mode)
{
    if (mode == new_mode) return;
    mode = new_mode;
}

void RTRender::setRealtimeMode() { changeMode(REALTIME); }
void RTRender::setPrerenderMode() { changeMode(PRERENDER); }

void RTRender::setFramesNumber(int frames_number)
{
    if (frames_number < 1 || mode == REALTIME || state == RENDERING) return;
    if (frames != nullptr)
        removeFrames(frames);
    frames = nullptr;

    this->frames_number = frames_number;
    createFrames(frames_number);
}

Mode RTRender::getMode() { return mode; }
uint RTRender::getFramesNumber() { return frames_number; }

void RTRender::updateCamera() { _precomputeValues(); }
