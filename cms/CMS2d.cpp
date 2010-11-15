#include "CMS2d.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace CMS2D
{
  /* Continuous model synthesis main function
  */
  void continuousModelSynthesis2D(vector<Edge*> &edges, vector<Vertex*> &verticies)
  {
    for( std::vector<Vertex*>::iterator vertex_itr = verticies.begin();
      vertex_itr != verticies.end(); vertex_itr++)
    {
      //NULL out unassigned edges (already done)
      //for(int itr = (*vertex_itr)->currentEdge; itr < 4; itr++)
      //  (*vertex_itr)->edges[itr] = NULL;
      //Prepare edge list for use in CMS2d
      sortEdges(*vertex_itr);
    }

    std::vector<VertexState> sourceValidStates;
    std::vector<VertexState> validStates;
    generateValid(sourceValidStates);
    generateStates( verticies, sourceValidStates, validStates);
    srand((unsigned int)time(NULL));
    while(validStates.size() > 0)
    {
      std::vector<VertexState>::iterator itr = validStates.begin();
      int size = validStates.size();
      int randomstate = rand()%(validStates.size());
      itr += randomstate;
      VertexState state = (*itr);
      validStates.erase(itr);
      for(int i = 0; i < 4; i++)
      {
        if(state.dependentedges[i] != NULL)
        {
          constrainEdge(state.dependentedges[i], state.dependentstates[i].leftFace,
          state.dependentstates[i].rightFace, validStates);
        }
      }
    }
  }

  /* Takes in a list of verticies,
  * a list of valid states from source,
  * and an empty list to put state in,
  * and populates the empty list with
  * all possible valid states for all vertecies.
  */
  void generateStates(vector<Vertex*> &verticies,
    vector<VertexState> &sourceValidStates,
    vector<VertexState> &validStates)
  {
    validStates.clear();
    for(std::vector<Vertex*>::iterator vertex_itr = verticies.begin();
      vertex_itr != verticies.end(); vertex_itr++)
    {
      Vertex *current = *vertex_itr;
      int sets[2];
      if(current->edges[0] != NULL)
        sets[0] = current->edges[0]->edgestate.set;
      else
        sets[0] = current->edges[1]->edgestate.set;
      if(current->edges[2] != NULL)
        sets[1] = current->edges[2]->edgestate.set;
      else
        sets[1] = current->edges[3]->edgestate.set;

      for(std::vector<VertexState>::iterator source_itr = sourceValidStates.begin();
        source_itr != sourceValidStates.end(); source_itr++)
      {
        ;
        if((*source_itr).setintersection[0] == sets[0] &&
          (*source_itr).setintersection[1] == sets[1] )
        {
          validStates.push_back(VertexState());
          if(current->edges[0] != NULL)
            validStates.back().setintersection[0] = current->edges[0]->edgestate.set;
          else
            validStates.back().setintersection[0] = current->edges[1]->edgestate.set;

          if(current->edges[2] != NULL)
            validStates.back().setintersection[1] = current->edges[2]->edgestate.set;
          else
            validStates.back().setintersection[1] = current->edges[3]->edgestate.set;

          validStates.back().dependentedges[0] = current->edges[0];
          validStates.back().dependentedges[1] = current->edges[1];
          validStates.back().dependentedges[2] = current->edges[2];
          validStates.back().dependentedges[3] = current->edges[3];
          validStates.back().dependentstates[0].leftFace =
            (*source_itr).dependentstates[0].leftFace;
          validStates.back().dependentstates[0].rightFace =
            (*source_itr).dependentstates[0].rightFace;
          validStates.back().dependentstates[1].leftFace =
            (*source_itr).dependentstates[1].leftFace;
          validStates.back().dependentstates[1].rightFace =
            (*source_itr).dependentstates[1].rightFace;
          validStates.back().dependentstates[2].leftFace =
            (*source_itr).dependentstates[2].leftFace;
          validStates.back().dependentstates[2].rightFace =
            (*source_itr).dependentstates[2].rightFace;
          validStates.back().dependentstates[3].leftFace =
            (*source_itr).dependentstates[3].leftFace;
          validStates.back().dependentstates[3].rightFace =
            (*source_itr).dependentstates[3].rightFace;
        }
      }
    }
  }

  /* Populates stateList with a list of valid states
  * currently creates a pre-defined list
  * but latter will take in a model that describes possible states
  */
  void generateValid(vector<VertexState> &stateList)
  {
    stateList.clear();

    
    stateList.push_back(VertexState());
    stateList.back().setintersection[0] = 0;
    stateList.back().setintersection[1] = 1;
    stateList.back().dependentstates[0].leftFace = INTERIOR;
    stateList.back().dependentstates[0].rightFace = INTERIOR;
    stateList.back().dependentstates[1].leftFace = INTERIOR;
    stateList.back().dependentstates[1].rightFace = INTERIOR;
    stateList.back().dependentstates[2].leftFace = INTERIOR;
    stateList.back().dependentstates[2].rightFace = INTERIOR;
    stateList.back().dependentstates[3].leftFace = INTERIOR;
    stateList.back().dependentstates[3].rightFace = INTERIOR;

    stateList.push_back(VertexState());
    stateList.back().setintersection[0] = 0;
    stateList.back().setintersection[1] = 1;
    stateList.back().dependentstates[0].leftFace = INTERIOR;
    stateList.back().dependentstates[0].rightFace = INTERIOR;
    stateList.back().dependentstates[1].leftFace = INTERIOR;
    stateList.back().dependentstates[1].rightFace = INTERIOR;
    stateList.back().dependentstates[2].leftFace = INTERIOR;
    stateList.back().dependentstates[2].rightFace = EXTERIOR;
    stateList.back().dependentstates[3].leftFace = INTERIOR;
    stateList.back().dependentstates[3].rightFace = EXTERIOR;


    stateList.push_back(VertexState());
    stateList.back().setintersection[0] = 1;
    stateList.back().setintersection[1] = 2;
    stateList.back().dependentstates[0].leftFace = INTERIOR;
    stateList.back().dependentstates[0].rightFace = EXTERIOR;
    stateList.back().dependentstates[1].leftFace = INTERIOR;
    stateList.back().dependentstates[1].rightFace = EXTERIOR;
    stateList.back().dependentstates[2].leftFace = INTERIOR;
    stateList.back().dependentstates[2].rightFace = INTERIOR;
    stateList.back().dependentstates[3].leftFace = INTERIOR;
    stateList.back().dependentstates[3].rightFace = INTERIOR;
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

  /* Take in an edge, the state parameters we will set it to 
  * and a list of all the valid vertexStates.
  * Sets the edge to the constrained state and removes
  * invalid vertex states that do not conform to this edge
  */
  void constrainEdge(Edge *edge, FaceState left, FaceState right,
    vector<VertexState> &vertexStates)
  {
    boolean statevalid;
    //Change the edge to the contstrained state
    edge->edgestate.leftFace = left;
    edge->edgestate.rightFace = right;
    vector<VertexState>::iterator itr = vertexStates.begin();

    //For all vertex states
    while(itr != vertexStates.end())
    {
      statevalid = true;
      //For all edges in the vertex states
      for(int i = 0; i < 4; i++)
      {
        //see if the edge is the edge we are concidering
        if((*itr).dependentedges[i] == edge &&
          // If so, then see if th edge does not conform to the constrained state
          (edge->edgestate.leftFace != (*itr).dependentstates[i].leftFace ||
          edge->edgestate.rightFace != (*itr).dependentstates[i].rightFace ))
        {
          //vertex is now an invalid vertex
          statevalid = false;
          continue;
        }
      }
      if (statevalid == false)
        // Remove invalid vertex state from the vertexStates list
        itr = vertexStates.erase(itr);
      else
        // Or continue
        itr++;
    }
  }
}