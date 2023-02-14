#ifndef PINTA_TRANSFORMATIONS_H
#define PINTA_TRANSFORMATIONS_H

#include "matrix.h"

namespace pinta {

Matrix ortho(float left, float right, float bottom, float top, float near, float far);
Matrix translate(float x, float y, float z);

}

#endif
