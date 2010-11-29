/**
    \class Grid
    \brief Keeps track of all the edges and vertices
*/

#ifndef _GRID_H
#define _GRID_H

#include "Geometry.h"
#include "CMSModel3D.h"
#include "CMSModel.h"
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
    vector<Face*>   faces;
    vector<Volume*> volumes;
    Edge          **parallelEdges;      ///< Keeps track of the parallel lines that intersect the bounding box
    int            *numEdges;           ///< Keeps track of how many edges there are per set of parallel edges
    int             numModelEdges;      ///< Number of edges the model has.  Need for deallocation of resources.
    float           spacing;            ///< The spacing between the parallel lines
    vector<vector<Face*> > parallelFaces;


    
    typedef std::tr1::unordered_map<unsigned long long,Edge*> EdgeHash;
    typedef std::tr1::unordered_map<unsigned long long,Vertex*> VertexHash;
    EdgeHash fastEdges;
    VertexHash fastVertices;

    void sortVertices3D(vector<Vertex*> &vertices, int left, int right, const Cuboid &bb);
    void sortFaces(vector<Face*> &faces, int left, int right);

private:
     bool edgeAlreadyInList(Edge *edge);
     Vertex *findVertex(Vertex *v);
     Face *findFace(Vertex* v1, Vertex *v2, Vertex *v3, Vertex *v4);
     bool assignEdge(Vertex* begin, Vertex *end);
     
      //template <typename Iterator>
      //inline bool next_combination(const Iterator first, Iterator k, const Iterator last);

      template <class BidirectionalIterator>
bool next_combination(BidirectionalIterator first, BidirectionalIterator k,
                      BidirectionalIterator last);


};

#endif // _GRID_H