#include "rtvector.h"
#include <cmath>

RTVector::RTVector()
    : RTPoint(0, 0, 0) {}

RTVector::RTVector(float x, float y, float z, bool normalize)
    : RTPoint(x, y, z) { if (normalize) this->normalize(); }

void RTVector::normalize()
{
    auto length = this->getLength();
    if (fabs(length) > 1e-5)
    {
        x /= length;
        y /= length;
        z /= length;
    }
}

float RTVector::getLength() const
{
    return sqrtf(x * x + y * y + z * z);
}

float RTVector::getAngle(const RTVector &v1, const RTVector &v2)
{
    float len_v1 = v1.getLength();
    float len_v2 = v2.getLength();
    float dot_prod = RTVector::dotProduct(v1, v2);
    float cos_alpha = dot_prod / (len_v1 * len_v2);

    return acos(cos_alpha);
}

float RTVector::dotProduct(const RTVector &v1, const RTVector &v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

RTVector RTVector::crossProduct(const RTVector &v1, const RTVector &v2)
{
    return RTVector(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}


RTVector operator+(const RTVector &v1, const RTVector &v2) { return RTVector(v2.x + v1.x, v2.y + v1.y, v2.z + v1.z); }
RTVector operator-(const RTVector &v1, const RTVector &v2) { return RTVector(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z); }
RTVector operator+(const RTVector &v, const RTPoint &p) { return RTVector(v.x + p.x, v.y + p.y, v.z + p.z); }
RTVector operator-(const RTVector &v, const RTPoint &p) { return RTVector(v.x - p.x, v.y - p.y, v.z - p.z); }


void RTVector::operator+=(RTVector &v1) { *this = *this + v1; }
void RTVector::operator*=(float n) { *this = *this*n; }

RTVector RTVector::operator*(float n) { return RTVector(this->x*n, this->y*n, this->z*n); }

bool operator==(const RTVector &v1, const RTVector &v2) { return (fabs(v1.x - v2.x) < 1e-5) && (fabs(v1.y - v2.y) < 1e-5)  && (fabs(v1.z - v2.z) < 1e-5) ; }
