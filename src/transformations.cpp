
#include "pinta/transformations.h"

namespace pinta {

Matrix orthoMatrix(float left, float right, float bottom, float top, float near, float far)
{
    float deltaX = right - left;
    float deltaY = top - bottom;
    float deltaZ = far - near;

    Matrix matrix = Matrix::identity();
    matrix[0][0] = 2.0 / deltaX;
    matrix[3][0] = -(right + left) / deltaX;
    matrix[1][1] = 2.0 / deltaY;
    matrix[3][1] = -(top + bottom) / deltaY;
    matrix[2][2] = -2.0 / deltaZ;
    matrix[3][2] = -(near + far) / deltaZ;

    return matrix;
}

Matrix translationMatrix(const Vector2 &position)
{
    Matrix matrix = Matrix::identity();
    matrix[3][0] = position.getX();
    matrix[3][1] = position.getY();
    matrix[3][2] = 0.0;
    return matrix;
}

}
