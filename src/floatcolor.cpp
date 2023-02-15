
#include "pinta/floatcolor.h"

namespace pinta {

FloatColor::FloatColor(float r, float g, float b, float a):
    r(r), g(g), b(b), a(a)
{

}

FloatColor::FloatColor(const FloatColor &other):
    r(other.r), g(other.g), b(other.b), a(other.a)
{

}

const FloatColor & FloatColor::operator=(const FloatColor &other)
{
    if (this == &other)
        return *this;
    r = other.r;
    g = other.g;
    b = other.b;
    a = other.a;
    return *this;
}

}
