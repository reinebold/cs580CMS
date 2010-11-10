/*********************************************

File:           Main.cpp
Description:

**********************************************/

#include "Utilities.h"
#include "Camera.h"
#include "Controls.h"
#include "Lights.h"
#include "State.h"
#include "CMS.h"

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
    
    cms.display();

    camera.printInfo();
    glutSwapBuffers();

    Utils::printError();
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.0,0.0,0.0,0.0);
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
    //Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
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
        cerr << "Error: OpenGL version 3.3 not supported" << endl;
        std::exit(EXIT_FAILURE);
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
