/**
    \class  Lights
    \brief  Controls some simple lighting functions
*/

#ifndef _LIGHTS_H
#define _LIGHTS_H

#include "Defines.h"
#define MAX_LIGHTS 8

///Struct that contains properties of a light
struct Light
{
    GLfloat position[4];
    GLfloat color[4];
    GLfloat attenuation[3];
};

class Lights
{
public:
    Lights();
    ///Add a light to the program
    void addLight(GLfloat x, GLfloat y, GLfloat z, GLfloat w = 1.0f, GLfloat r = 1.0f, GLfloat g = 1.0f, GLfloat b = 1.0f, GLfloat ca = 1.0f, GLfloat la = 0.0f, GLfloat qa = 0.0f);
    ///Renders all the lights that were added by addLight
    void renderLights();

private:
    int numLights;                  ///< Total lights that were added
    Light lightInfo[MAX_LIGHTS];    ///< Keeps track of all the lights that were added
    Light ambient;                  ///< Global ambient value
};

#endif // _LIGHTS_H