/**
    \file CMSModel3D.h
    \brief
*/

#include "CMSModel3D.h"
#include "Geometry.h"
using namespace Geometry;

CMSModel3D::CMSModel3D()
:numFaces(0),
 numVertices(0),
 numEdges(0),
 edges(NULL),
 faces(NULL),
 vertices(NULL)
{

}

CMSModel3D::~CMSModel3D()
{
    delete [] vertices;
    delete [] edges;
    delete [] faces;
}

void CMSModel3D::init(int _numVertices, Vertex *_vertices, int _numFaces, Face *_faces)
{
    numFaces     = _numFaces;
    numVertices  = _numVertices;
    numEdges     = _numVertices;

    //Allocate memory
    vertices  = new Vertex[numVertices];
    edges     = new Edge[numEdges];
    faces     = new Face[numFaces];

    //Assign the vertices
    for(int x = 0; x < numVertices; ++x)
    {
        vertices[x] = _vertices[x];
    }

    //Assign the edges (based on the vertices)
    for(int x = 0; x < numFaces; ++x)
    {
        faces[x] = _faces[x];
        for(int y = 0; y < faces->numVertices; ++y)
        {
            int begin = x;
            int end;
            if(y+1 == faces->numVertices)
            {
                end = 0;
            }
            else
            {
                end = y+1;
            }

            //vertices[begin].faces[

            edges[y].begin = &vertices[begin];
            vertices[begin].edges[1] = &edges[y];

            edges[y].end = &vertices[end];
            vertices[end].edges[0] = &edges[y];

            edges[y].updateEdgeState();
        }
    }
}