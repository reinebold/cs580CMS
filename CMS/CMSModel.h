/**
    \class CMSModel
    \brief Contains information for the model that was given via the text file.
*/

#ifndef _CMSMODEL_H
#define _CMSMODEL_H

#include "Geometry.h"
using namespace Geometry;

class CMSModel
{
public:
    CMSModel();     
    ~CMSModel();    ///< Deallocates vertices and edges

    ///Fills in the data members.
    void init(int _numFaces, int _numVertices, Vertex *_vertices);
   
    int        numVertices;      ///< The number of vertices of the model
    int        numEdges;          ///< The number of edges of the model
    int        numFaces;          ///< The number of faces of the model
    Vertex    *vertices;         ///< The vertices of the model
    Edge      *edges;             ///< The edges of the model
};


#endif // _CMSMODEL_H