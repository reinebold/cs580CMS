/**
    \class Model
    \brief This is a class for the 3ds model, still needs a lot of work
*/
#ifndef _MODEL_H
#define _MODEL_H

#include <iostream>
#include <GL/lib3ds.h>
#include <GL/glew.h>

class Model
{
public:
    Model();
    ~Model();
    void render();
    void loadModel(std::string filename);
    void getCamera(float camera[9]);

private:
    Lib3dsFile * load_lib3ds_file(const char *filestr);
    float (*vertices)[3], (*normals)[3];
    float boundingBoxMin[3];
    float boundingBoxMax[3]; 
    unsigned int numFaces;
    Lib3dsFile * model;
    GLuint vboVertices;
    GLuint vboNormals;
};

#endif // _MODEL_H