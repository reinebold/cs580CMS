/**
    \file CMSModel.h
    \brief
*/

#include "CMSModel.h"

CMSModel::CMSModel()
{

}

CMSModel::~CMSModel()
{
    delete [] verticies;
    delete [] edges;
}

void CMSModel::init(int _numVerticies, Vertex *_verticies)
{
    numVerticies = _numVerticies;

    //TODO: Lots of error checking.

    //Allocate memory
    verticies = new Vertex[numVerticies];
    edges = new Edge[numVerticies];

    //Assign the verticies
    for(int x = 0; x < numVerticies; ++x)
    {
        verticies[x] = _verticies[x];
    }

    //Assign the edges (based on the verticies)
    for(int x = 0; x < numVerticies; ++x)
    {
        int begin = x;
        int end;
        if(x+1 == numVerticies)
        {
            end = 0;
        }
        else
        {
            end = x+1;
        }

        edges[x].begin = &verticies[begin];
        edges[x].end = &verticies[end];
        edges[x].updateEdgeState();
    }
}