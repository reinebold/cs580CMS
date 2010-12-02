/**
    \file   Texture.cpp
    \brief
*/

#include <IL/il.h>
#include <IL/ilu.h>
//#include "glew.h"

#include <iostream>
using namespace std;

#include "Texture.h"

Texture::Texture()
{
    ilInit();
}   

Texture::~Texture()
{
    ilShutDown();
}

void Texture::loadTexture(const char * fileName)
{
    ILuint imageIndex;

    if (!fileName || fileName[0] == '\0') 
    {
        cerr << "ERROR: Passed in NULL filename for texture.\n";
        exit(EXIT_FAILURE);
    }

    ilGenImages(1, &imageIndex);
    ilBindImage(imageIndex);
    if (!ilLoadImage((wchar_t *)fileName))
    {
        ilDeleteImages(1, &imageIndex);
        ilShutDown();
        cerr << "ERROR: Could not load texture: " << fileName << ".\n";
        exit(EXIT_FAILURE);
    }
    if (ilGetInteger(IL_ORIGIN_MODE) == IL_ORIGIN_LOWER_LEFT) 
    {
        iluFlipImage();
    }

    width    = (unsigned int)ilGetInteger(IL_IMAGE_WIDTH);
    height   = (unsigned int)ilGetInteger(IL_IMAGE_HEIGHT);
    channels = (unsigned int)ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    data     = ilGetData();
    devilid  = imageIndex;

    if      (channels == 1) { format = GL_LUMINANCE; }
    else if (channels == 3) { format = GL_RGB; }
    else if (channels == 4) { format = GL_RGBA; }
}

void Texture::uploadTexture()
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id); 
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST ); 
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, format, GL_UNSIGNED_BYTE, data );
    destroyTexture();
}

void Texture::destroyTexture()
{
    ilDeleteImages(1, &devilid);
}