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
    ~Grid();    ///< Deallocates parallelEdges, numEdges, vertices, and edges
    
    void init3D(const CMSModel &model,const Cuboid &boundingbox);
    void init2D(const CMSModel &model,const Cuboid &boundingbox);
    void init(const CMSModel &model,const Cuboid &boundingbox);             ///< Will call either 2d or 3d depending on the number of faces of the input model
    void sortVertices(vector<Vertex*> &vertices, int left, int right);    ///< Simple quicksort

    vector<Vertex*> vertices;          ///< The vertices of the intersections of the parallelEdges
    vector<Edge*>   edges;              ///< The split up edges of the parallel lines
    Edge          **parallelEdges;      ///< Keeps track of the parallel lines that intersect the bounding box
    int            *numEdges;           ///< Keeps track of how many edges there are per set of parallel edges
    int             numModelEdges;      ///< Number of edges the model has.  Need for deallocation of resources.
    float           spacing;            ///< The spacing between the parallel lines
};

#endif // _GRID_H