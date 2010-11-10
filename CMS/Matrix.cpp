/**
    \file   Matrix.cpp
    \brief
*/

#include "Matrix.h"
#include <iostream>
using namespace std;

void Matrix::printMatrix(GLfloat matrix[])
{
   cout << matrix[0] << "\t"
        << matrix[4] << "\t"
        << matrix[8] << "\t"
        << matrix[12]<< "\t"
        << endl
        << matrix[1] << "\t"
        << matrix[5] << "\t"
        << matrix[9] << "\t"
        << matrix[13]<< "\t"
        << endl
        << matrix[2] << "\t"
        << matrix[6] << "\t"
        << matrix[10]<< "\t"
        << matrix[14]<< "\t"
        << endl
        << matrix[3] << "\t"
        << matrix[7] << "\t"
        << matrix[11]<< "\t"
        << matrix[15]<< "\t"
        << endl << endl;
}