/**
    \class Grid
    \brief Keeps track of all the edges and vertices
*/

#ifndef _GRID_H
#define _GRID_H

#include "Geometry.h"
#include <vector>
using namespace Geometry;

class Grid
{
public:
    Grid();
    ~Grid();    ///< Deallocates parallelEdges and numEdges
    
    void init(const CMSModel &model,const Cuboid &boundingbox);             ///< End result will get all the verticies and edges needed
    void sortVerticies(vector<Vertex*> &verticies, int left, int right);    ///< Simple quicksort

    vector<Vertex*> verticies;          ///< The verticies of the intersections of the parallelEdges
    vector<Edge*>   edges;              ///< The split up edges of the parallel lines
    Edge          **parallelEdges;      ///< Keeps track of the parallel lines that intersect the bounding box
    int            *numEdges;           ///< Keeps track of how many edges there are per set of parallel edges
    int             numModelEdges;         ///< Number of edges the model has.  Need for deallocation of resources.
    float           spacing;            ///< The spacing between the parallel lines
};

#endif // _GRID_H