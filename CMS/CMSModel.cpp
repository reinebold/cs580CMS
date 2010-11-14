

#include "CMSModel.h"

void CMSModel::init(int numVerticies, Vertex *verticies)
{
    _numVerticies = numVerticies;

    //TODO: Lots of error checking.


    //Allocate memory
    _verticies = new Vertex[_numVerticies];
    _edges = new Edge[_numVerticies];

    //Assign the verticies
    for(int x = 0; x < _numVerticies; ++x)
    {
        _verticies[x] = verticies[x];
    }

    //Assign the edges (based on the verticies)
    for(int x = 0; x < _numVerticies; ++x)
    {
        int begin = x;
        int end;
        if(x+1 == _numVerticies)
        {
            end = 0;
        }
        else
        {
            end = x+1;
        }

        _edges[x].begin = &_verticies[begin];
        _edges[x].end = &_verticies[end];

        _edges[x].edgestate.slope.num = (_verticies[end].val[Y] - _verticies[begin].val[Y]);
        _edges[x].edgestate.slope.den = (_verticies[end].val[X] - _verticies[begin].val[X]);
        _edges[x].edgestate.slope.value = _edges[x].edgestate.slope.num/_edges[x].edgestate.slope.den;
        //TODO: Figure out how to find out if the left side or right side is exterior or interior
        _edges[x].edgestate.leftFace = INTERIOR;
        _edges[x].edgestate.rightFace = EXTERIOR;
    }
}