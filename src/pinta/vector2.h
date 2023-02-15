#ifndef PINTA_VECTOR2_H
#define PINTA_VECTOR2_H

namespace pinta {

class Vector2 {

public:

    Vector2();
    Vector2(float x, float y);

    inline float getX() const {return x;}
    inline float getY() const {return y;}

private:

    float x;
    float y;

};

}

#endif