#ifndef RTRENDER_H
#define RTRENDER_H

#include "rtpoint.h"
#include "rtray.h"

#include "rtscene.h"
#include "rtcanvas.h"
#include "rtcamera.h"

typedef enum Mode {
    REALTIME,
    PRERENDER
} Mode;

typedef enum State {
    RENDERING,
    READY
} State;

class RTRender
{
public:
    RTRender(RTScene *cube, RTCanvas *canvas);

    void resetScene();

    void renderFrame();
    void paint(int frame_number = 0);

    void setRealtimeMode();
    void setPrerenderMode();
    void setFramesNumber(int frames_number);

    uint getFramesNumber();
    Mode getMode();

    void updateCamera();


    RTCanvas *canvas;
    RTCamera *camera;

    RTColor backgroundColor;
    RTVector lightSource;

    RTColor **frameBuffer;

    float light_coeff;
    bool lightOn;

    State state;

private:
    RTScene *scene;
    Mode mode;
    void changeMode(Mode new_mode);

    RTColor ***frames;
    uint frames_number;
    uint frames_rendered;

    void createFrames(uint n);
    void removeFrame(RTColor **frame);
    void removeFrames(RTColor ***frames);

    RTPoint canvasCoordToWorldPoint(int x, int y);
    bool traceRay_3DDDA(RTRay &ray, int &r, int &g, int &b, RTPoint &point);
    RTColor light(const RTColor &color, float angle, float diffuse_k = 0);

    void _renderFrame(uint id_start, uint id_end, RTColor **frame);

    // precomputed values, used during rendering
    void _precomputeValues();

    RTRay **_rays;

    uint _canvas_width;
    uint _canvas_height;

    float _canvas_width_half;
    float _canvas_height_half;

    float _hratio;
    float _vratio;

    float _cos_hangle;
    float _sin_hangle;
    float _cos_vangle;
    float _sin_vangle;
};

#endif // RTRENDER_H
