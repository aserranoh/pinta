#ifndef PINTA_FLOATCOLOR_H
#define PINTA_FLOATCOLOR_H

namespace pinta {

class FloatColor {

public:

    FloatColor(float r, float g, float b, float a = 1.0);
    FloatColor(const FloatColor &other);

    const FloatColor & operator=(const FloatColor &other);

    inline float getRed() const {return r;}
    inline float getGreen() const {return g;}
    inline float getBlue() const {return b;}
    inline float getAlpha() const {return a;}

private:

    float r;
    float g;
    float b;
    float a;

};

}

#endif
