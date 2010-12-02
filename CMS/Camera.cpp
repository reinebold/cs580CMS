/**
    \file   Camera.cpp
    \brief
*/

#include "Camera.h"
#include "Defines.h"
#include "Matrix.h"
#include <iostream>
using namespace std;

extern Camera camera;

Camera::Camera()
   :znear(1.0f),
    zfar(1000.f),
    fovy(45.0f),
    windowWidth(WINDOW_WIDTH),
    windowHeight(WINDOW_HEIGHT),
    translatex(0.0f),
    translatey(0.0f),
    translatez(0.0f),
    rotatex(0.0f),
    rotatey(0.0f),
    rotatez(0.0f),
    scale(1.0f),
    aspect((float)windowWidth/(float)windowHeight)
{
	position.x = 15.0f;
	position.y =45.0f;
	position.z = 90.0f;
	look.x = 0.0f;
	look.y = -0.2f;
	look.z = -1.0f;
  look.normalize();
	up.x=0.0f;
	up.y=1.0f;
	up.z=0.0f;
}

void Camera::printInfo() const
{
    static bool doOnce = false;

    if(!doOnce)
    {
        doOnce = true;
        cout << "Camera Information" << endl;
        cout << "------------------" << endl;
        GLfloat matrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
        Matrix::printMatrix(matrix);
        glGetFloatv(GL_PROJECTION_MATRIX, matrix);
        Matrix::printMatrix(matrix);

        printTransformations();
    }
}

void Camera::printTransformations() const
{
    cout << "Translation:   " << translatex << "\t"
                              << translatey << "\t"
                              << translatez << "\t" << endl
         << "Rotation:      " << rotatex    << "\t"
                              << rotatey    << "\t"
                              << rotatez    << "\t" << endl
         << "Scale:         " << scale      << "\t" << endl;
}

void Camera::moveCamera()
{
    setPerspectiveProjectionMatrix();
    setModelViewMatrix();
 
    //glTranslatef(translatex,translatey,translatez);
    //glRotatef(rotatex, 1.0f, 0.0f, 0.0f);
    //glRotatef(rotatey, 0.0f, 1.0f, 0.0f);
    //glRotatef(rotatez, 0.0f, 0.0f, 1.0f);
    //glScalef(scale, scale, scale);
}

void Camera::changeWindowSize(GLint width, GLint height)
{
    if (height == 0)
    {
        height = 1;
    }

    windowWidth  = width;
    windowHeight = height;
    aspect       = static_cast<float>(windowWidth)/static_cast<float>(windowHeight);
    
    setPerspectiveProjectionMatrix();
}

void Camera::setOrthoProjectionMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-(GLfloat)windowWidth/2.0f,
             (GLfloat)windowWidth/2.0f,
            -(GLfloat)windowHeight/2.0f,
             (GLfloat)windowHeight/2.0f,
             1.0f,
             1000.0f);
}

void Camera::setPerspectiveProjectionMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, aspect, znear, zfar);
}

void Camera::setModelViewMatrix()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //gluLookAt(0,0,30,0,0,0,0,1,0);
    //gluLookAt(20.0f,25,40,20,25,0,0,1,0);
	gluLookAt(position.x, position.y, position.z, position.x + look.x, position.y + look.y, position.z + look.z, up.x, up.y, up.z);
}

void Camera::setModelViewMatrix(GLfloat posx, GLfloat posy, GLfloat posz, GLfloat lookx, GLfloat looky, GLfloat lookz)
{
	setModelViewMatrix();
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //gluLookAt(posx,posy,posz,lookx,looky,lookz,0,1,0);
}

void Camera::moveTo3DSCamera( Model &model )
{
    float camera[9];
    model.getCamera(camera);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera[6], aspect, 1.0, 20000000000);
	
	setModelViewMatrix();

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //gluLookAt(camera[0],camera[1],camera[2],camera[3],camera[4],camera[5],0,1,0);
   
    //glTranslatef(translatex,translatey,translatez);
    //glRotatef(rotatex, 1.0f, 0.0f, 0.0f);
    //glRotatef(rotatey, 0.0f, 1.0f, 0.0f);
    //glRotatef(rotatez, 0.0f, 0.0f, 1.0f);
    //glScalef(scale, scale, scale);
}



	void Camera::pitch(float degrees)
	{
		look.y -= degrees/100.0f;
		look.normalize();
		//setModelViewMatrix();
	}
	void Camera::yaw(float degrees)
	{
		Geometry::Vector movement;
		movement = look.crossProduct(up);
		movement.normalize();
		look += movement*(degrees/100.0f);
		look.normalize();
		//setModelViewMatrix();
	}
	void Camera::forwardb(float ammount)
	{
		position += look*(ammount);
		//setModelViewMatrix();
	}
	void Camera::upd(float ammount)
	{
		position += up*(ammount);
		//setModelViewMatrix();
	}
	void Camera::leftr(float ammount)
	{
		Geometry::Vector movement;
		movement = look.crossProduct(up);
		movement.normalize();
		position += movement*(-ammount);
	}