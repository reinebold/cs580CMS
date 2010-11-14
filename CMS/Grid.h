
#ifndef _GRID_H
#define _GRID_H

#include "Geometry.h"
#include <vector>
using namespace Geometry;

class Grid
{
public:
    Grid()
        :spacing(4.0f)     //TODO: For spacing, maybe find max distance between all points then times .75?
    {

    }

    ~Grid() 
    {
        //delete [] verticies;
    }
    
    void init(const CMSModel &model,const Cuboid &boundingbox);
    void sortVerticies(vector<Vertex*> &verticies, int left, int right);

    Cuboid  boundingbox;
    Edge  **parallelEdges;
    vector<Vertex*> verticies;
    vector<Edge*>   edges;
    int     numEdges[500];
    int     totalVerts;
    float   spacing;
};


#endif // _GRID_H