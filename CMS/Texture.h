/**
    \class Texture
    \brief Basically a wrapper to the library 'DevIL' to load in textures.
*/

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <gl/glew.h>

class Texture
{
public:
    Texture();
    ~Texture();

    ///Loads in a texture when a filename is specified
    void loadTexture(const char *fileName);

    ///Destroys the texture (good to use after a glTexImage2D call to save memory)
    void destroyTexture();
    void uploadTexture();

    //Gets
    unsigned int getId() const       { return id; }
    GLenum getFormat() const         { return format; }
    unsigned int getWidth() const    { return width; }
    unsigned int getHeight() const   { return height; }
    unsigned int getChannels() const { return channels; }
    unsigned char * getData() const  { return data; }

    //Sets
    unsigned int* setId() { return &id; }  ///< For use for something like glGenTextures(1, texture.setId())

private:
    GLenum        format;   ///< How the data is grouped, i.e. GL_RGB, GL_RGBA, etc
    unsigned int  width;    ///< Width of the image
    unsigned int  height;   ///< Height of the image
    unsigned int  channels; ///< Number of channels of the image (i.e. 3 for RGB
    unsigned int  devilid;  ///< Keeps track of the DevIL id so it can properly be destroyed
    unsigned int  id;       ///< Texture id (received from glGenTextures
    unsigned char *data;    ///< The actual texture data
};

#endif // _TEXTURE_H