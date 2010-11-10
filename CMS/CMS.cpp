/**
    \file   CMS
    \brief  
*/

#include "CMS.h"
#include "Lights.h"
#include "State.h"
#include "Program.h"

extern Lights lights;
extern State state;

CMS::CMS()
{

}

void CMS::display()
{
    glDisable(GL_TEXTURE_2D);
    if(state.getTest() == 0) //Use normal coloring
    {
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    else if(state.getTest() == 1)   //Use a shader program that colors everything teal
    {
        glUseProgram(programID);
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texWater.getId());
    }
   
    GLfloat size = 10.0;
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, -size);     //top
        glTexCoord2f(1.0f, 1.0f); glVertex3f( size, size, -size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( size,  size, size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size,  size, size);

        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(size, size, size);       //right
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, -size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size,-size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(size, -size, size);

        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, -size);     //far
        glTexCoord2f(1.0f, 1.0f); glVertex3f( size, size, -size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size,-size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size,  -size, -size);

        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, size);      //front
        glTexCoord2f(1.0f, 1.0f); glVertex3f( size, size, size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size, size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, size);

        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, -size);     //left
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-size,  size, size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size, size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, -size);

        glNormal3f(0.0f,-1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, -size, size);     //bottom
        glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  -size, size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size,  -size, -size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size,  -size, -size);
    glEnd();

    glBindTexture(GL_TEXTURE_2D,0);
    glUseProgram(0);
}

void CMS::init()
{
    state.setWireFrame(false);
    state.setLighting(true);
    state.setDrawLights(true);
    state.setDrawAxis(false);
    state.setPrintInfoOnScreen(true);

    lights.addLight( 15.0,15.0,0.0, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    lights.addLight(-20.0,5.0, 0.0, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);

    //Load program (for when t = 1)
    Program program;
    programID = program.createProgram("simple.vs", "simple.fs");

    //Load texture (for when t = 2)
    texWater.loadTexture(RESOURCE_DIR "water.jpg");
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); //instead of GL_MODULATE, we do GL_REPLACE so that it won't multiply
    glGenTextures(1, texWater.setId());                             //the texture color with the primitive color.
    glBindTexture(GL_TEXTURE_2D, texWater.getId());
    glTexImage2D(GL_TEXTURE_2D, 0, texWater.getFormat(), texWater.getWidth(), texWater.getHeight(), 0, texWater.getFormat(), GL_UNSIGNED_BYTE, texWater.getData());
    texWater.destroyTexture();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
    glBindTexture(GL_TEXTURE_2D, 0);
}