#ifndef PINTA_TRANSFORMATIONS_H
#define PINTA_TRANSFORMATIONS_H

#include "pinta/matrix.h"
#include "pinta/vector2.h"

namespace pinta {

Matrix orthoMatrix(float left, float right, float bottom, float top, float near, float far);
Matrix translationMatrix(const Vector2 &vector);

}

#endif
