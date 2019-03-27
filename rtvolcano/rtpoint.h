#ifndef RTPOINT_H
#define RTPOINT_H

class RTPoint
{
public:
    float x, y, z;

    RTPoint();
    explicit RTPoint(float x, float y, float z);

    RTPoint operator+(const RTPoint &p);
    RTPoint& operator+=(const RTPoint &v1);
    RTPoint& operator=(const RTPoint &p);

    static void getIntValues(RTPoint &point, int &x, int &y, int &z);
    static int getIntValue(float f);

    void debugPrint();
    void debugPrint() const;

};

#endif // RTPOINT_H
