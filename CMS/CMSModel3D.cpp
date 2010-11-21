/**
    \file CMSModel3D.h
    \brief
*/

#include "CMSModel3D.h"
#include "Geometry.h"
#include "Utilities.h"
using namespace Geometry;

CMSModel3D::CMSModel3D()
:numFaces(0),
 numVertices(0),
 faces(NULL),
 vertices(NULL)
{

}

CMSModel3D::~CMSModel3D()
{
    delete [] vertices;
    delete [] faces;
}

void CMSModel3D::init(int _numVertices, Vertex *_vertices, int _numFaces, Face *_faces)
{
    numFaces     = _numFaces;
    numVertices  = _numVertices;

    vertices = _vertices;
    faces = _faces;

    colors = new Vertex[numFaces];
    for(int x = 0; x < numFaces; x++)
    {
        colors[x] = Utils::randColor(0.5f);
    }
    

    //Allocate memory
   // vertices  = new Vertex[numVertices];
   // faces     = new Face[numFaces];

    //Assign the vertices
    /*for(int x = 0; x < numVertices; ++x)
    {
        vertices[x] = _vertices[x];
    }

    for(int x = 0; x < numFaces; ++x)
    {
        faces[x] = _faces[x];
        for(int y = 0; y < faces[x].numVertices; ++y)
        {
           faces[x].vertices = _faces[x]
            //vertices[begin].faces[vertices->connectedFaces++] = 

           // edges[y].begin = &vertices[begin];
           // vertices[begin].edges[1] = &edges[y];

          //  edges[y].end = &vertices[end];
           // vertices[end].edges[0] = &edges[y];

          //  edges[y].updateEdgeState();
        }
    }*/
}