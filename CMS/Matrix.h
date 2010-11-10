/**
    \class Matrix
    \brief Does matrix operations
*/

#include <GL/glew.h>

#ifndef _MATRIX_H
#define _MATRIX_H

class Matrix
{
public:
    static void printMatrix(GLfloat matrix[]);  ///< Assumes 4x4 Matrix (GLfloat matrix[16])
};

#endif // _MATRIX_H