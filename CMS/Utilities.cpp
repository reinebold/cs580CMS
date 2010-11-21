/**
    \file   Utilities.cpp
    \brief
*/

#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include "Defines.h"
#include "Camera.h"
#include "Utilities.h"
#include "CMS.h"
using namespace std;

extern Camera camera;
extern CMS cms;

template <typename T>
string Utils::numToString(T value)
{
    std::stringstream out;
    out << value;
    return out.str();
}

void Utils::printStatus()
{
    cout << "OpenGL Information" << endl;
    cout << "------------------" << endl;
    cout << "OpenGL version:\t" << glGetString(GL_VERSION) << endl; 
    //cout << "Shader version:\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "Vendor:\t\t" << glGetString(GL_VENDOR) << endl;
    cout << "Renderer:\t" << glGetString(GL_RENDERER) << endl;
    cout << "GLEW version:\t" << glewGetString(GLEW_VERSION)<< endl; 
    cout << endl;
}

void Utils::printError()
{
    static bool errorsAlreadyShown = false;
    GLenum error;
    
    error = glGetError();
    if((errorsAlreadyShown == false) && (error != GL_NO_ERROR))
    {
        errorsAlreadyShown = true;
        cout << "ERROR Detected" << endl;
        cout << "---------------" << endl;
        
        int count = 0;
        while(error != GL_NO_ERROR)
        {
            cout << "Error " << ++count << ": ";
            
            switch(error)
            {
            case GL_INVALID_ENUM:
                cout << "GL_INVALID_ENUM\t" << "(" << error << ")" << endl;
                break;
            case GL_INVALID_VALUE:
                cout << "GL_INVALID_VALUE" << " (" << error << ")" << endl;
                break;
            case GL_INVALID_OPERATION:
                cout << "GL_INVALID_OPERATION" << " (" << error << ")" << endl;
                break;
            case GL_STACK_OVERFLOW:
                cout << "GL_STACK_OVERFLOW" << " (" << error << ")" << endl;
                break;
            case GL_STACK_UNDERFLOW:
                cout << "GL_STACK_UNDERFLOW" << " (" << error << ")" << endl;
                break;
            case GL_OUT_OF_MEMORY:
                cout << "GL_OUT_OF_MEMORY" << " (" << error << ")" << endl;
                break;
            case GL_TABLE_TOO_LARGE:
                cout << "GL_TABLE_TOO_LARGE" << " (" << error << ")" << endl;
                break;
            default:
                cout << "UNKNOWN ERROR" << " (" << error << ")" << endl;
                break;
            }
            error = glGetError();
        }
        cout << endl;
    }
}

void Utils::printInfoOnScreen()
{   
    GLfloat count = 10.0;
    glColor3f(1.0f,1.0f,1.0f);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D( 0,
                    camera.getWindowWidth(),
                    0,
                    camera.getWindowHeight());
        glScalef(1,-1,1);
        glTranslatef(0.0f,-(GLfloat)camera.getWindowHeight(),0.0f);
           
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            renderString(0,count,"-Transformations-");
            count += 10;
            renderString(0,count,"Rotate x: " + numToString(camera.getRotatex()));
            count += 10;
            renderString(0,count,"Rotate y: " + numToString(camera.getRotatey()));
            count += 10;
            renderString(0,count,"Rotate z: " + numToString(camera.getRotatez()));
            count += 10;
            renderString(0,count,"Scale:    " + numToString(camera.getScale()));
            count += 10;
            renderString(0,count,"Trans x:  " + numToString(camera.getTranslatex()));
            count += 10;
            renderString(0,count,"Trans y:  " + numToString(camera.getTranslatey()));
            count += 10;
            renderString(0,count,"Trans z:  " + numToString(camera.getTranslatez()));
            count += 20;
            renderString(0,count,"-CMS-");
            count += 10;
            renderString(0,count,"Verticies: " + numToString(cms.grid.verticies.size()));
            count += 10;
            renderString(0,count,"Edges:     " + numToString(cms.grid.edges.size()));
            count += 10;
            renderString(0,count,"Spacing:   " + numToString(cms.grid.spacing));
            count += 10;
        glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void Utils::centerWindow(int &x, int &y)
{
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    x = (screenWidth - WINDOW_WIDTH)/2;
    y = (screenHeight - WINDOW_HEIGHT)/2;
}

void Utils::drawAxis()
{
    glBegin(GL_LINES);
        //Y
        glColor3f(1.0f,0.0f,0.0f);
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(0.0f,1.0f,0.0f);

        //X
        glColor3f(0.0f,1.0f,0.0f);
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(1.0f,0.0f,0.0f);

        //Z
        glColor3f(0.0f,0.0f,1.0f);
        glVertex3f(0.0f,0.0f,0.0f);
        glVertex3f(0.0f,0.0f,1.0f);
    glEnd();
}

void Utils::renderString(GLfloat x, GLfloat y, std::string text)
{
    glRasterPos2f(x,y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)text.c_str());
}

float Utils::randFloat(float a, float b)
{
    return ((b-a)*((float)rand()/RAND_MAX))+a;
   
}

int Utils::randInt(int low, int high)
{
    //make sure to seed random number generator.
    return rand()%(high-low) + low;
}