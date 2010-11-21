/**
    \namespace Utils
    \brief     All utility functions go here such as renderString and printError
*/

#ifndef _UTILITIES_H
#define _UTILITIES_H

#include "Geometry.h"
#include <string>
#include <GL/glew.h>

namespace Utils
{
    ///Prints OpenGL version, GLEW version, etc in the command window
    void printStatus();

    ///Prints an error to the command window if something comes up.  This is called everytime int he main window.
    void printError();

    ///Prints various helpful information to the actual window like transformations
    void printInfoOnScreen();

    ///Centers the window on the screen
    void centerWindow(int &x, int &y);

    ///Draws axis on window
    void drawAxis();

    ///Renders a string in an orthographic projection
    void renderString(GLfloat x, GLfloat y, std::string text);
    
    ///Returns a number to a string
    template <typename T>
    std::string numToString(T value);

    float randFloat(float a, float b);

    int randInt(int low, int high);

    void quicksortWithInsertion(int *a, int left, int right);

    Geometry::Vertex randColor(float alpha = 1.0f);
}

#endif // _UTILITIES_H