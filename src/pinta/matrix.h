#ifndef PINTA_MATRIX_H
#define PINTA_MATRIX_H

namespace pinta {

class Matrix {

public:

    static Matrix identity();

    Matrix();
    Matrix(const Matrix &other);
    ~Matrix();

    const Matrix & operator=(const Matrix &other);
    Matrix operator*(const Matrix& other) const;
    float * operator[](int index);

    const float * data() const;

private:

    float m[4][4];

};

}

#endif