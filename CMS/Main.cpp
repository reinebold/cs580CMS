/*********************************************

File:           Main.cpp
Description:

**********************************************/

#include <iostream>

#include "Utilities.h"
#include "Camera.h"
#include "Controls.h"
#include "Lights.h"
#include "State.h"
#include "CMS.h"
#include "City.h"
#include "Geometry.h"

Camera      camera;
Lights      lights;
State       state;
Controls    controls;
CMS         cms;

void display()
{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    if(state.getPrintInfoOnScreen())
    {
        Utils::printInfoOnScreen();
    }
    camera.moveCamera();
    
    if(state.getDrawAxis())
    {
        Utils::drawAxis();
    }

    if(state.getLighting())
    {
        lights.renderLights();
    }

    if(state.getWireFrame())
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    if(cms.input.numFaces == 1)
    {
         cms.display2D();
    }
    else
    {
         cms.display3D();
    }

    glutSwapBuffers();
    Utils::printError();
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.0,0.0,0.0,1.0);
    glClearDepth(1.0f);
    glShadeModel(GL_SMOOTH);    //Or GL_FLAT
    cms.init();
}
void reshape(GLsizei width, GLsizei height)
{
    glViewport(0, 0, width, height);
    camera.changeWindowSize(width, height);
}

void main(int argc, char **argv)
{
	/*City c;
	c.setPopulation(4);
	c.setDevelopment(1);

	int gridWidth = 2;
	int gridHeight = 2;

	int** grid = c.getGrid(gridWidth, gridHeight);

	for(int i=0; i < gridWidth; i++) {
		for(int j=0; j < gridHeight; j++) {
			std::cout << grid[i][j];
		}
		std::cout << endl;
	}

	for(int i=0; i < gridWidth; i++) {
		delete grid[i];
	}
	delete grid;*/

	//Geometry::Vector a(1, 2, 3);
	//Geometry::Vector b(0, -1, 2);
	//Geometry::Vector c;
	//c = a.crossProduct(b);
	//std::cout << a.dotProduct(b) << std::endl;

    //Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(camera.getWindowWidth(), camera.getWindowHeight());
    int x = 0, y = 0;
    Utils::centerWindow(x,y);
    glutInitWindowPosition(x, y);
    glutCreateWindow(WINDOW_TITLE);

    // Initialize GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        cerr << "Error: " << glewGetErrorString(err) << endl;
        std::exit(EXIT_FAILURE);
    }
    if (!glewIsSupported("GL_VERSION_3_3"))
    {
        cerr << "Warning: OpenGL version 3.3 not supported with GLEW" << endl;
    }

    Utils::printStatus();

    init();
 
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMouseFunc(Controls::mousePressHandler);
    glutMotionFunc(Controls::mouseMotionHandler);
    glutKeyboardFunc(Controls::keyboardHandler);

    glutMainLoop();
}
