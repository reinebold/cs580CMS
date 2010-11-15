#include "CMS2d.h"

void continuousModelSynthesis2D(vector<Edge*> &edges, vector<Vertex*> &verticies)
{
  for( std::vector<Vertex*>::iterator vertex_itr = verticies.begin();
    vertex_itr != verticies.end(); vertex_itr++)
  {
    //NULL out unassigned edges
    for(int itr = (*vertex_itr)->currentEdge; itr < 4; itr++)
      (*vertex_itr)->edges[itr] = NULL;
    //Prepare edge list for use in CMS2d
    sortEdges(*vertex_itr);
  }

  vector<VertexState> validStates;
  for(std::vector<Vertex*>::iterator vertex_itr = verticies.begin();
    vertex_itr != verticies.end(); vertex_itr++)
  {
    Vertex *current = *vertex_itr;
    validStates.push_back(VertexState());
    if(current->edges[0] != NULL)
      validStates.back().setintersection[0] = current->edges[0]->edgestate.set;
    else if( current->edges[1] != NULL)
      validStates.back().setintersection[0] = current->edges[1]->edgestate.set;

    if(current->edges[2] != NULL)
      validStates.back().setintersection[1] = current->edges[2]->edgestate.set;
    else if( current->edges[3] != NULL)
      validStates.back().setintersection[1] = current->edges[3]->edgestate.set;
    
    validStates.back().dependentedges[0] = current->edges[0];
    validStates.back().dependentedges[1] = current->edges[1];
    validStates.back().dependentedges[2] = current->edges[2];
    validStates.back().dependentedges[3] = current->edges[3];
  }
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