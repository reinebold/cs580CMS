/**
    \file CMSModel.h
    \brief
*/

#include "CMSModel.h"

CMSModel::CMSModel()
:numFaces(0),
 numVertices(0),
 numEdges(0)
{

}

CMSModel::~CMSModel()
{
    delete [] vertices;
    delete [] edges;
}

void CMSModel::init(int _numFaces, int _numVertices, Vertex *_vertices)
{
    numFaces     = _numFaces;
    numVertices  = _numVertices;
    numEdges     = _numVertices;

    //TODO: Lots of error checking.

    //Allocate memory
    vertices = new Vertex[numVertices];
    edges = new Edge[numVertices];

    //Assign the vertices
    for(int x = 0; x < numVertices; ++x)
    {
        vertices[x] = _vertices[x];
    }

    //Assign the edges (based on the vertices)
    for(int x = 0; x < numVertices; ++x)
    {
        int begin = x;
        int end;
        if(x+1 == numVertices)
        {
            end = 0;
        }
        else
        {
            end = x+1;
        }

        edges[x].begin = &vertices[begin];
        vertices[begin].edges[1] = &edges[x];

        edges[x].end = &vertices[end];
        vertices[end].edges[0] = &edges[x];

        edges[x].updateEdgeState();
    }
}