#ifndef _CMS2D_H
#define _CMS2D_H

#include <GL/glew.h>
#include "CMSModel.h"
#include "Geometry.h"
#include "Grid.h"

namespace CMS2D
{
  class VertexState
  {
  public:
    int setintersection[2];
    Edge *dependentedges[4];
    EdgeState dependentstates[4];
  };

  void continuousModelSynthesis2D(vector<Edge*> &edges, vector<Vertex*> &verticies);
  void generateStates(vector<Vertex*> &verticies, vector<VertexState> &source,
    vector<VertexState> &stateList);
  void generateValid(vector<VertexState> &stateList);
  void sortEdges(Vertex *v);
  void constrainEdge(Edge *edge, FaceState left, FaceState right,
    vector<VertexState> &vertexStates);
}
#endif