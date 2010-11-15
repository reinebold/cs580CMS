#include "CMS2d.h"

void continuousModelSynthesis2D(vector<Edge*> &edges, vector<Vertex*> &verticies)
{

}

/* 
* sotrEdges( Vertex *v)
* Sorts the edges in a vertex edge list based on
* Lower set first, then by
* inbound edge (vertex is edges end) first
*/
void sortEdges(Vertex *v)
{
  Edge *temps[4];
  int minset = INT_MAX;

  for(int itr = 0 ; itr < 4; itr++)
  {
    temps[itr] = v->edges[itr];
    if(temps[itr] != NULL)
      if(temps[itr]->edgestate.set < minset)
        minset = temps[itr]->edgestate.set;
    v->edges[itr] = NULL;
  }

  for(int itr = 0; itr < 4; itr++)
  {
    if(temps[itr] == NULL) continue;
    if(temps[itr]->edgestate.set == minset)
    {
      if(temps[itr]->end == v)
      {
        v->edges[0] = temps[itr];
      }
      else
      {
        v->edges[1] = temps[itr];
      }
    }
    else
    {
      if(temps[itr]->end == v)
      {
        v->edges[2] = temps[itr];
      }
      else
      {
        v->edges[3] = temps[itr];
      }
    }
  }

  // now v.edges[#] is
  // 0 = lower set, inbound edge
  // 1 = lower set, outbound edge
  // 2 = upper set, inbound edge
  // 3 = upper set, outbound edge
}