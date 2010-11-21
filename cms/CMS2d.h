#ifndef _CMS2D_H
#define _CMS2D_H

#include "CMSModel.h"
#include "Geometry.h"
#include "Grid.h"

namespace CMS2D
{
  class VertexStateEdges
  {
  public:
    VertexStateEdges(int seta, int setb);
    VertexStateEdges(const VertexStateEdges &other);
    bool operator<(VertexStateEdges &rhs);
    bool operator==(VertexStateEdges &rhs);
    int setintersection[2];
    EdgeState dependentstates[4];
  };

  class VertexState
  {
  public:
    VertexState(int *relCount, VertexStateEdges edges, Vertex *parent);
    VertexState(const VertexState &other);
    bool operator<(VertexState &rhs);
    Edge *dependentedges[4];
    int *relativesCounter;
    VertexStateEdges edgeinfo;
    Vertex *parent;
  };

  bool continuousModelSynthesis2D(vector<Edge*> &edges, vector<Vertex*> &vertices,
    CMSModel &input, Grid &grid);
  void generateValid(vector<VertexStateEdges> &stateList,  CMSModel &input, Grid &grid);
  bool concaveTest(Edge *a, Edge *b);
  void generateStates(vector<Vertex*> &vertices, vector<VertexStateEdges> &source,
    vector<VertexState> &stateList, int *relativesCounter);
  void sortEdges(Vertex *v);
  void constrainEdge(Edge *edge, FaceState left, FaceState right,
    vector<VertexState> &vertexStates);
}
#endif