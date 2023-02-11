
#include "matrix.h"
#include <cstring>

namespace pinta {

Matrix Matrix::identity()
{
    Matrix matrix;
    memset(matrix.m, 0, sizeof(matrix.m));
    for (int i = 0; i < 4; i++) {
        matrix.m[i][i] = 1.0;
    }
    return matrix;
}

Matrix::Matrix():
    m{}
{

}

Matrix::~Matrix()
{

}

const Matrix & Matrix::operator=(const Matrix &other)
{
    if (&other == this)
        return *this;
    memcpy(m, other.m, sizeof(m));
    return *this;
}

Matrix Matrix::operator*(const Matrix& other) const {
    Matrix result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

float * Matrix::operator[](int index)
{
    return m[index];
}

const float * Matrix::data() const
{
    return &m[0][0];
}

}