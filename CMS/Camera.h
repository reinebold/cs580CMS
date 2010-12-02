/**
    \class  Camera
    \brief  Keeps track of camera attributes, and has a few functions that set 
            the modelview matrix and projection matrix
*/

#ifndef _CAMERA_H
#define _CAMERA_H

#include "Model.h"
#include "Defines.h"
#include "Geometry.h"

class Camera
{
public:
    Camera();

    ///Sets the modelview, projection, and multiplies with any transformations
    void moveCamera();
    ///Prints the projection, modelview, and initial transformation values to command window
    void printInfo() const;
    ///Prints the initial transformation values to the command window
    void printTransformations() const;

    ///TODO: Doesn't work exactly as I want it to
    void moveTo3DSCamera(Model &model);
    
    void setModelViewMatrix();              ///< Sets the modelview matrix to the default
    void setOrthoProjectionMatrix();        ///< Sets the projection matrix to use an orthographic projection
    void setPerspectiveProjectionMatrix();  ///< Sets the projection matrix to use a perspective projection
    void changeWindowSize(GLint width, GLint height); ///< Changes the window size
    void setModelViewMatrix(GLfloat posx, GLfloat posy, GLfloat posz, GLfloat lookx, GLfloat looky, GLfloat lookz);

    //Gets
    GLfloat getRotatez() const      { return rotatez; }
    GLint   getWindowHeight() const { return windowHeight; }
    GLint   getWindowWidth() const  { return windowWidth; }
    GLfloat getZnear() const        { return znear; }
    GLfloat getZfar() const         { return zfar; }
    GLfloat getFovy() const         { return fovy; }
    GLfloat getAspect() const       { return aspect; }
    GLfloat getTranslatex() const   { return translatex; }
    GLfloat getTranslatey() const   { return translatey; }
    GLfloat getTranslatez() const   { return translatez; }
    GLfloat getRotatex() const      { return rotatex; }
    GLfloat getRotatey() const      { return rotatey; }
    GLfloat getScale() const        { return scale; }

    //Sets
    void setWindowWidth(GLint val)  { windowWidth = val; }
    void setWindowHeight(GLint val) { windowHeight = val; }
    void setZnear(GLfloat val)      { znear = val; }
    void setZfar(GLfloat val)       { zfar = val; }
    void setFovy(GLfloat val)       { fovy = val; }
    void setAspect(GLfloat val)     { aspect = val; }
    void setTranslatex(GLfloat val) { translatex = val; }
    void setTranslatey(GLfloat val) { translatey = val; }
    void setRotatex(GLfloat val)    { rotatex = val; }
    void setTranslatez(GLfloat val) { translatez = val; }
    void setRotatey(GLfloat val)    { rotatey = val; }
    void setRotatez(GLfloat val)    { rotatez = val; }
    void setScale(GLfloat val)      { scale = val; }
    
private:
    GLint   windowWidth;    ///< Current window width (NOT screen width)
    GLint   windowHeight;   ///< Current window height (NOT screen height)
    GLfloat znear;          ///< z near clipping plane (for use with gluPerspective)
    GLfloat zfar;           ///< z far clipping plane (for use with gluPerspective)
    GLfloat fovy;           ///< Field of view in the y direction (for use with gluPerspective)
    GLfloat aspect;         ///< aspect ratio of the window (for use with gluPerspective)
    
    GLfloat scale;          ///< Tells what to scale the camera by
    GLfloat translatex;     ///< Tells what to translate the camera by in the x direction
    GLfloat translatey;     ///< Tells what to translate the camera by in the y direction
    GLfloat translatez;     ///< Tells what to translate the camera by in the z direction
    GLfloat rotatex;        ///< Tells what to rotate the camera by in the x direction
    GLfloat rotatey;        ///< Tells what to rotate the camera by in the y direction
    GLfloat rotatez;        ///< Tells what to rotate the camera by in the z direction

	

public:
	void pitch(float degrees);
	void yaw(float degrees);
	void forwardb(float ammount);
	void upd(float ammount);
	void leftr(float ammount);

    Geometry::Vector position;
	Geometry::Vector look;
	Geometry::Vector up;

};

#endif // _CAMERA_H