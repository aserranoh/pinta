#ifndef PINTA_COLOR_H
#define PINTA_COLOR_H

#include <cstdint>

namespace pinta {

class Color {

public:

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    Color(const Color &other);

    const Color & operator=(const Color &other);

    inline uint8_t getRed() const {return r;}
    inline uint8_t getGreen() const {return g;}
    inline uint8_t getBlue() const {return b;}
    inline uint8_t getAlpha() const {return a;}

private:

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

};

}

#endif
