/**
    \class Grid
    \brief Keeps track of all the edges and vertices
*/

#ifndef _GRID_H
#define _GRID_H

#include "Geometry.h"
#include "CMSModel3D.h"
#include <vector>
#include <unordered_map>
using namespace Geometry;

class Grid
{
public:
    Grid();
    ~Grid();    ///< Deallocates parallelEdges, numEdges, vertices, and edges
    
    void init(const CMSModel &model,const Cuboid &boundingbox);             ///< Will call either 2d or 3d depending on the number of faces of the input model
    void init(const CMSModel3D &model,const Cuboid &boundingbox);             ///< Will call either 2d or 3d depending on the number of faces of the input model
    void sortVertices(vector<Vertex*> &vertices, int left, int right);    ///< Simple quicksort

    vector<Vertex*> vertices;          ///< The vertices of the intersections of the parallelEdges
    vector<Edge*>   edges;              ///< The split up edges of the parallel lines
    Edge          **parallelEdges;      ///< Keeps track of the parallel lines that intersect the bounding box
    int            *numEdges;           ///< Keeps track of how many edges there are per set of parallel edges
    int             numModelEdges;      ///< Number of edges the model has.  Need for deallocation of resources.
    float           spacing;            ///< The spacing between the parallel lines
    vector<vector<Face*> > parallelFaces;

    
    typedef std::tr1::unordered_map<unsigned long long,Edge*> EdgeHash;
    EdgeHash fastEdges;
    void sortVertices3D(vector<Vertex*> &vertices, int left, int right, const Cuboid &bb);

private:
     bool edgeAlreadyInList(Edge *edge);
     bool assignEdge(Vertex* begin, Vertex *end);
     
      //template <typename Iterator>
      //inline bool next_combination(const Iterator first, Iterator k, const Iterator last);

      template <class BidirectionalIterator>
bool next_combination(BidirectionalIterator first, BidirectionalIterator k,
                      BidirectionalIterator last);


};

#endif // _GRID_H