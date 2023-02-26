#ifndef PINTA_FLOATANIMATION_H
#define PINTA_FLOATANIMATION_H

namespace pinta {

class FloatAnimation {

public:

    FloatAnimation(float startValue, float endValue, float time, float ticksPerSecond);

    float getValue() const;
    void start();
    void step();

private:

    float startValue;
    float endValue;
    float time;
    float ticksPerSecond;
    float currentValue;

};

}

#endif