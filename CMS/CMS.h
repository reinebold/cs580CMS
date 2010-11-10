/**
    \class CMS
    \brief Class to start creating the continuous model synthesis
*/

#include <GL/glew.h>
#include "Texture.h"

class CMS
{
public:
    CMS();

    void display();
    void init();

private:
    GLuint programID;
    Texture texWater;

};