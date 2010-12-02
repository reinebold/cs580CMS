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
        //camera.setRotatex( fmod(camera.getRotatex() + (y - controls.getOldy()), 360) );
        //camera.setRotatey( fmod(camera.getRotatey() + (x - controls.getOldx()), 360) ); 
		camera.pitch((y - controls.getOldy())/2.0f);
		camera.yaw((x - controls.getOldx())/2.0f);
    }
    // Translate Camera
    else if(controls.getLeftButton() == false && controls.getRightButton() == true)
    {
        //camera.setTranslatex( camera.getTranslatex() +  .1f * (x - controls.getOldx()) ); 
        //camera.setTranslatey( camera.getTranslatey() + -.1f * (y - controls.getOldy()) ); 
		camera.upd(-(y - controls.getOldy())/5.0f);
		camera.leftr(-(x - controls.getOldx())/5.0f);
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

void Controls::specialKeyHandler(int key, int x, int y){
	switch(key){
		case GLUT_KEY_UP:
			//camera.setRotatex( camera.getRotatex() + 10);
			camera.forwardb(2.0f);
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			//camera.setRotatex( camera.getRotatex() - 10);
			camera.forwardb(-2.0f);
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			//camera.setRotatey( camera.getRotatey() + 10);
			camera.leftr(2.0f);
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			//camera.setRotatey( camera.getRotatey() - 10);
			camera.leftr(-2.0f);
			glutPostRedisplay();
			break;
	}
}

void Controls::keyboardHandler(unsigned char key, int x, int y)
{
    switch(key)
    {	
	case 'z':
			camera.setTranslatez( camera.getTranslatez() + -.8f );
			glutPostRedisplay();
			break;
	case 'x':
			camera.setTranslatez( camera.getTranslatez() + .8f);
			glutPostRedisplay();
			break;
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
        /*state.setWireFrame(!state.getWireFrame());
		state.setTest(4); 
        cms.showBoundingBox = !cms.showBoundingBox;
		cms.showGridEdges = !cms.showGridEdges;
		cms.showGridVertices = !cms.showGridVertices;
		cms.showGridFaces = !cms.showGridFaces;
		cms.showGridVolumes = !cms.showGridVolumes;*/
        if(cms.showTexture == false)
        {
            //glEnable(GL_TEXTURE_2D);
            cms.showGridEdges = false;
            cms.showGridVertices = false;
            cms.showGridFaces = false;
            cms.showBoundingBox = false;
            cms.showGridVolumes = true;
            cms.showTexture = true;
            cms.showModel = false;
            //state.setLighting(true);
        }
        else
        {
            //state.setLighting(false);
            cms.showModel = true;
            cms.showGridVolumes = true;
            cms.showBoundingBox = true;
            cms.showTexture = false;
            //glDisable(GL_TEXTURE_2D);
        }
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
        if(state.getTest() == cms.grid.parallelFaces.size()+1) 
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
    case 'e':
        cms.showGridEdges = !cms.showGridEdges;
        glutPostRedisplay();
        break;
    case 'f':
        cms.showGridFaces = !cms.showGridFaces;
        glutPostRedisplay();
        break;
    case 'c':
        cms.showGridVolumes = !cms.showGridVolumes;
        glutPostRedisplay();
        break;
    case 'h':
        cms.recreateCity = true;
        glutPostRedisplay();
        break;
    case 'm':
        cms.showModel = !cms.showModel;
        glutPostRedisplay();
        break;
    }
}