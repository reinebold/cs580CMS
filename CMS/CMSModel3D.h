/**
    \class CMSModel3D
    \brief Contains information for the model that was given via the text file.
*/

#ifndef _CMSMODEL3D_H_
#define _CMSMODEL3D_H_

#include "Geometry.h"
using namespace Geometry;

class CMSModel3D
{
public:
    CMSModel3D();     
    ~CMSModel3D();    ///< Deallocates vertices and edges

    ///Fills in the data members.
    void init(int _numVertices, Vertex *_vertices, int _numFaces, Face *_faces);
   
    int        numVertices;      ///< The number of vertices of the model
    int        numFaces;          ///< The number of faces of the model
    Vertex    *vertices;         ///< The vertices of the model, by face
    Face      *faces;             ///< The faces of the model
    Vertex    *colors;           ///< The colors of the input model.     
};

#endif // _CMSMODEL3D_H_