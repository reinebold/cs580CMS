/**
    \file   Controls.cpp
    \brief  
*/

#include "Controls.h"
#include "Camera.h"
#include "State.h"
#include "Defines.h"
#include "CMS.h"
#include <GL/freeglut.h>
#include <math.h>       //for fmod()

extern Camera camera;
extern State state;
extern Controls controls;
extern CMS cms;

void Controls::mousePressHandler(int button, int state, int x, int y) 
{
    if ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON))
    {
        controls.setLeftButton(true);
        controls.setOldx(x);
        controls.setOldy(y);
    }
    else if((state == GLUT_UP) && (button == GLUT_LEFT_BUTTON))
    {
        controls.setLeftButton(false);
    }

    else if((state == GLUT_DOWN) && (button == GLUT_RIGHT_BUTTON))
    {
        controls.setOldx(x);
        controls.setOldy(y);
        controls.setRightButton(true);
    }
    else if((state == GLUT_UP) && (button == GLUT_RIGHT_BUTTON))
    {
        controls.setRightButton(false);
    }

    glutPostRedisplay();
}

void Controls::mouseMotionHandler(int x, int y)
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport );

    // Rotate Camera
    if(controls.getLeftButton() == true && controls.getRightButton() == false)
    {
        //We fmod it by 360 because it is easier to see that it rotated 360 degrees instead of 720 degrees.
        camera.setRotatex( fmod(camera.getRotatex() + (y - controls.getOldy()), 360) );
        camera.setRotatey( fmod(camera.getRotatey() + (x - controls.getOldx()), 360) ); 
    }
    // Translate Camera
    else if(controls.getLeftButton() == false && controls.getRightButton() == true)
    {
        camera.setTranslatex( camera.getTranslatex() +  .1f * (x - controls.getOldx()) ); 
        camera.setTranslatey( camera.getTranslatey() + -.1f * (y - controls.getOldy()) ); 
    }
    // Scale Camera
    else if(controls.getLeftButton() == true && controls.getRightButton() == true)
    {
        camera.setScale( camera.getScale() + float(y-controls.getOldy())/viewport[3] ); 
    }

    controls.setOldy(y);
    controls.setOldx(x);

    glutPostRedisplay();
}

void Controls::keyboardHandler(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'i':   //Prints the top left information on the screen
        state.setPrintInfoOnScreen(!state.getPrintInfoOnScreen());
        glutPostRedisplay();
        break;
    case ESC_KEY:   //Exits the program
        exit(EXIT_SUCCESS);
        break;
    case 'l':   //Turns on/off lights
        state.setLighting(!state.getLighting());
        glutPostRedisplay();
        break;
    case 'a':   //Turns on/off axis
        state.setDrawAxis(!state.getDrawAxis());
        glutPostRedisplay();
        break;
    case 'w':   //Turns on/off wireframe mode
        state.setWireFrame(!state.getWireFrame());
        glutPostRedisplay();
        break;
    case 'd':
        state.setDrawLights(!state.getDrawLights());
        glutPostRedisplay();
        break;
    case 'r':   //Resets all transformations
        camera.setTranslatex(0.0f);
        camera.setTranslatey(0.0f);
        camera.setTranslatez(0.0f);
        camera.setScale(1.0f);
        camera.setRotatex(0.0f);
        camera.setRotatey(0.0f);
        camera.setRotatez(0.0f);
        glutPostRedisplay();
        break;
    case 't':
        if(state.getTest() == 2) 
        { 
            state.setTest(0); 
        }
        else
        {
            state.setTest(state.getTest() + 1);
        }
        glutPostRedisplay();
        break;
    case 'b':
        cms.showBoundingBox = !cms.showBoundingBox;
        glutPostRedisplay();
        break;
    case 'v':
        cms.showGridVertices = !cms.showGridVertices;
        glutPostRedisplay();
        break;
    }
}