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
    int setintersection[2];
    EdgeState dependentstates[4];
  };

  class VertexState
  {
  public:
    VertexState(int *relCount, VertexStateEdges edges);
    bool operator<(VertexState &rhs);
    Edge *dependentedges[4];
    int *relativesCounter;
    VertexStateEdges edgeinfo;
  };

  void continuousModelSynthesis2D(vector<Edge*> &edges, vector<Vertex*> &verticies);
  void generateStates(vector<Vertex*> &verticies, vector<VertexStateEdges> &source,
    vector<VertexState> &stateList, int *relativesCounter);
  void generateValid(vector<VertexStateEdges> &stateList);
  void sortEdges(Vertex *v);
  void constrainEdge(Edge *edge, FaceState left, FaceState right,
    vector<VertexState> &vertexStates);
}
#endif