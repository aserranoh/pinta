
#include "pinta/color.h"

namespace pinta {

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a):
    r(r), g(g), b(b), a(a)
{

}

Color::Color(const Color &other):
    r(other.r), g(other.g), b(other.b), a(other.a)
{

}

const Color & Color::operator=(const Color &other)
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
