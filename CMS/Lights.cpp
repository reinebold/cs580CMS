/**
    \file   Lights.cpp
    \brief 
*/

#include "Lights.h"
#include "State.h"

extern State state;

Lights::Lights()
:numLights(-1)
{
    ambient.color[X] = 0.2f;
    ambient.color[Y] = 0.2f;
    ambient.color[Z] = 0.2f;
    ambient.color[W] = 1.0f;
}

void Lights::addLight(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat r, GLfloat g, GLfloat b,
                      GLfloat ca,GLfloat la,GLfloat qa)
{
    if(numLights < MAX_LIGHTS)
    {
        numLights++;
        lightInfo[numLights].position[X] = x;
        lightInfo[numLights].position[Y] = y;
        lightInfo[numLights].position[Z] = z;
        lightInfo[numLights].position[W] = w;

        //Warning: Don't forget, after the first light the color defaults to 0,0,0,0
        lightInfo[numLights].color[X] = r;
        lightInfo[numLights].color[Y] = g;
        lightInfo[numLights].color[Z] = b;
        lightInfo[numLights].color[W] = 1.0f;

        lightInfo[numLights].attenuation[CONSTANT_ATTENUATION]  = ca;
        lightInfo[numLights].attenuation[LINEAR_ATTENUATION]    = la;
        lightInfo[numLights].attenuation[QUADRATIC_ATTENUATION] = qa;
    }
}

void Lights::renderLights()
{
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient.color);

    for(int i = 0; i <= numLights; ++i)
    {
        //Draw a small dot to show where the light source is.
        if(state.getDrawLights() && lightInfo[i].position[W] == 1.0f)
        {
            glDisable(GL_LIGHTING);
            glEnable(GL_POINT_SMOOTH);  //Make the point a sphere basically.
            glPointSize(2.0f);          //Change the size of the point

            glBegin(GL_POINTS);
                glColor3f(lightInfo[i].color[X], lightInfo[i].color[Y], lightInfo[i].color[Z]);
                glVertex3f(lightInfo[i].position[X],lightInfo[i].position[Y],lightInfo[i].position[Z]);
            glEnd();
            glDisable(GL_POINT_SMOOTH);
        }

        glEnable(GL_LIGHTING);
        //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); //basically turns all glColor calls into glMaterial calls for help lighting
        //glEnable(GL_COLOR_MATERIAL);
        glShadeModel(GL_SMOOTH);

        glEnable(GL_LIGHT0+i);
        glLightfv(GL_LIGHT0+i, GL_POSITION, lightInfo[i].position);
        glLightfv(GL_LIGHT0+i, GL_SPECULAR, lightInfo[i].color);
        glLightfv(GL_LIGHT0+i, GL_DIFFUSE,  lightInfo[i].color);
        glLightfv(GL_LIGHT0+i, GL_CONSTANT_ATTENUATION, &lightInfo[i].attenuation[CONSTANT_ATTENUATION]);
        glLightfv(GL_LIGHT0+i, GL_LINEAR_ATTENUATION, &lightInfo[i].attenuation[LINEAR_ATTENUATION]);
        glLightfv(GL_LIGHT0+i, GL_QUADRATIC_ATTENUATION, &lightInfo[i].attenuation[QUADRATIC_ATTENUATION]);
    }
}