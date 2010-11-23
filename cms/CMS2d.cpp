#include "CMS2d.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <time.h>
#include <limits>

namespace CMS2D
{
  /* Continuous model synthesis main function
  */
  bool continuousModelSynthesis2D(vector<Edge*> &edges, vector<Vertex*> &vertices,
    CMSModel &input,  Grid &grid)
  {
    for( std::vector<Vertex*>::iterator vertex_itr = vertices.begin();
      vertex_itr != vertices.end(); vertex_itr++)
      sortEdges(*vertex_itr);

    int *relativeCounters;
    std::vector<VertexStateEdges> sourceValidStates;
    std::vector<VertexState> validStates;
    relativeCounters = new int[vertices.size()];
    for(unsigned int i = 0; i < vertices.size(); i++)
      relativeCounters[i] = 0;
    generateValid(sourceValidStates, input, grid);
    generateStates( vertices, sourceValidStates, validStates, relativeCounters);
    //srand(time(NULL)); done already;
    while(validStates.size() > 0)
    {
      sort(validStates.begin(), validStates.end());
      std::vector<VertexState>::iterator itr = validStates.begin();
      if(*((*itr).relativesCounter) > 1)
      {
        int size = validStates.size();
        //int randomstate = rand()%(validStates.size());
        //Sanity check, validStates should be smaller than relativesCounter
        if( *((*itr).relativesCounter) > (int)validStates.size())
          exit(9001);
        int randomstate = rand() % *((*itr).relativesCounter);
        itr += randomstate;
      }
      else
      {
        int i = 0;
        i++;
      }
      //(*((*itr).relativesCounter))--;
      VertexState state = (*itr);
      validStates.erase(itr);
      for(int i = 0; i < 4; i++)
      {
        if(state.dependentedges[i] != NULL)
        {
          constrainEdge(state.dependentedges[i],
            state.edgeinfo.dependentstates[i].leftFace,
            state.edgeinfo.dependentstates[i].rightFace, validStates);
        }
      }
    }
    bool result = true;
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
      std::cout << relativeCounters[i];
      if( relativeCounters[i] == 0)
      {
        result = false;
      }
    }
    std::cout << std::endl;
    delete relativeCounters;
    return result;
  }

  /* Populates stateList with a list of valid states
  * currently creates a pre-defined list
  * but latter will take in a model that describes possible states
  */
  void generateValid(vector<VertexStateEdges> &stateList,  CMSModel &input,
    Grid &grid)
    //inputmodel is a counterclockwise list of vertices
  {
    stateList.clear();
    int numsets = 3;
    //For every lower, uppser set pair
    //genarate inside outside states that are always valid
    for(int x = 0; x < numsets; x++)
      for(int y = x+1; y < numsets; y++)
      {
        //Set All Interior state
        stateList.push_back(VertexStateEdges(x, y));
        stateList.back().dependentstates[0].leftFace = INTERIOR;
        stateList.back().dependentstates[0].rightFace = INTERIOR;
        stateList.back().dependentstates[1].leftFace = INTERIOR;
        stateList.back().dependentstates[1].rightFace = INTERIOR;
        stateList.back().dependentstates[2].leftFace = INTERIOR;
        stateList.back().dependentstates[2].rightFace = INTERIOR;
        stateList.back().dependentstates[3].leftFace = INTERIOR;
        stateList.back().dependentstates[3].rightFace = INTERIOR;
        //Set All Exterior state
        stateList.push_back(VertexStateEdges(x,y));
        stateList.back().dependentstates[0].leftFace = EXTERIOR;
        stateList.back().dependentstates[0].rightFace = EXTERIOR;
        stateList.back().dependentstates[1].leftFace = EXTERIOR;
        stateList.back().dependentstates[1].rightFace = EXTERIOR;
        stateList.back().dependentstates[2].leftFace = EXTERIOR;
        stateList.back().dependentstates[2].rightFace = EXTERIOR;
        stateList.back().dependentstates[3].leftFace = EXTERIOR;
        stateList.back().dependentstates[3].rightFace = EXTERIOR;

      }

      for(int itr = 0; itr < input.numVertices; itr++)
      {
        Vertex *curr = &(input.vertices[itr]);
        float inboundslope = curr->edges[0]->edgestate.slope.value;
        float outboundslope = curr->edges[1]->edgestate.slope.value;
        float inbounddiff = numeric_limits<float>::infinity();
        float outbounddiff = numeric_limits<float>::infinity();
        int inboundbestset = -1;
        int outboundbestset = -1;
        //Find best matching set for each inbound and outbound edges
        for(int itrg = 0; itrg < input.numVertices; itrg++)
        {
          if(abs(inboundslope-grid.parallelEdges[itrg]->edgestate.slope.value) < inbounddiff)
          {
            inbounddiff = abs(inboundslope-grid.parallelEdges[itrg]->edgestate.slope.value);
            inboundbestset = itrg;
          }
          if(abs(outboundslope-grid.parallelEdges[itrg]->edgestate.slope.value) < outbounddiff)
          {
            outbounddiff = abs(outboundslope-grid.parallelEdges[itrg]->edgestate.slope.value);
            outboundbestset = itrg;
          }
        }
        if(inboundbestset == outboundbestset)
          // Vertex is actually just a split edge
          continue;
        int sets[2];
        int inboundindex = 0;
        int outboundindex = 2;
        if(inboundbestset < outboundbestset)
        {
          sets[0] = inboundbestset;
          sets[1] = outboundbestset;
          inboundindex = 0;
          outboundindex = 2;
        }
        else
        {
          sets[1] = inboundbestset;
          sets[0] = outboundbestset;
          inboundindex = 2;
          outboundindex = 0;
        }
        bool inswap = false;
        bool outswap = false;
        bool concave = false;
        //concave = concaveTest(curr->edges[0], curr->edges[1]);

        //Case where vertex is an edge vertex aligned with inbound
        //inswap = (curr->edges[0]->edgestate.slope.den *
        //  grid.parallelEdges[inboundbestset]->edgestate.slope.den) < 0;
        //Assume grid lines are always pointed in positive X;
        inswap = (curr->edges[0]->edgestate.slope.den < 0);
        //outswap = (curr->edges[1]->edgestate.slope.den *
        //  grid.parallelEdges[outboundbestset]->edgestate.slope.den) < 0;
        //Assume grid lines are always pointed in positive X;
        outswap = (curr->edges[1]->edgestate.slope.den < 0);
        if(concave)
          outswap = !outswap;
        stateList.push_back(VertexStateEdges(sets[0],sets[1]));
        stateList.back().dependentstates[inboundindex].leftFace =
          (inswap ? EXTERIOR:INTERIOR);
        stateList.back().dependentstates[inboundindex].rightFace =
          (inswap ? INTERIOR:EXTERIOR);
        stateList.back().dependentstates[inboundindex+1].leftFace =
          (inswap ? EXTERIOR:INTERIOR);
        stateList.back().dependentstates[inboundindex+1].rightFace =
          (inswap ? INTERIOR:EXTERIOR);
        stateList.back().dependentstates[outboundindex].leftFace =
          (outswap ? INTERIOR:EXTERIOR);
        stateList.back().dependentstates[outboundindex].rightFace =
          (outswap ? INTERIOR:EXTERIOR);
        stateList.back().dependentstates[outboundindex+1].leftFace =
          (outswap ? EXTERIOR:INTERIOR);
        stateList.back().dependentstates[outboundindex+1].rightFace =
          (outswap ? EXTERIOR:INTERIOR);

        //Case where vertex is an edge vertex aligned with outbound
        //inswap = (curr->edges[0]->edgestate.slope.den *
        //  grid.parallelEdges[inboundbestset]->edgestate.slope.den) < 0;
        //Assume grid lines are always pointed in positive X;
        inswap = (curr->edges[0]->edgestate.slope.den < 0);
        if(concave) inswap = !inswap;
        //outswap = (curr->edges[1]->edgestate.slope.den *
        //  grid.parallelEdges[outboundbestset]->edgestate.slope.den) < 0;
        //Assume grid lines are always pointed in positive X;
        outswap = (curr->edges[1]->edgestate.slope.den < 0);
        stateList.push_back(VertexStateEdges(sets[0],sets[1]));
        stateList.back().dependentstates[outboundindex].leftFace =
          (outswap ? EXTERIOR:INTERIOR);
        stateList.back().dependentstates[outboundindex].rightFace =
          (outswap ? INTERIOR:EXTERIOR);
        stateList.back().dependentstates[outboundindex+1].leftFace =
          (outswap ? EXTERIOR:INTERIOR);
        stateList.back().dependentstates[outboundindex+1].rightFace =
          (outswap ? INTERIOR:EXTERIOR);
        stateList.back().dependentstates[inboundindex].leftFace =
          (inswap ? EXTERIOR:INTERIOR);
        stateList.back().dependentstates[inboundindex].rightFace =
          (inswap ? EXTERIOR:INTERIOR);
        stateList.back().dependentstates[inboundindex+1].leftFace =
          (inswap ? INTERIOR:EXTERIOR);
        stateList.back().dependentstates[inboundindex+1].rightFace =
          (inswap ? INTERIOR:EXTERIOR);

        //Now do corner as in model case.
        stateList.push_back(VertexStateEdges(sets[0], sets[1]));
        stateList.back().dependentstates[0].leftFace = concave ? INTERIOR : EXTERIOR;
        stateList.back().dependentstates[0].rightFace = concave ? INTERIOR : EXTERIOR;
        stateList.back().dependentstates[1].leftFace = concave ? INTERIOR : EXTERIOR;
        stateList.back().dependentstates[1].rightFace = concave ? INTERIOR : EXTERIOR;
        stateList.back().dependentstates[2].leftFace = concave ? INTERIOR : EXTERIOR;
        stateList.back().dependentstates[2].rightFace = concave ? INTERIOR : EXTERIOR;
        stateList.back().dependentstates[3].leftFace = concave ? INTERIOR : EXTERIOR;
        stateList.back().dependentstates[3].rightFace = concave ? INTERIOR : EXTERIOR;
        //assume grid's X direction is always positive
        inswap = (curr->edges[0]->edgestate.slope.den < 0);
        outswap = (curr->edges[1]->edgestate.slope.den < 0);
        if(inswap)
        {
          stateList.back().dependentstates[inboundindex+1].leftFace = EXTERIOR;
          stateList.back().dependentstates[inboundindex+1].rightFace = INTERIOR;
        }
        else
        {
          stateList.back().dependentstates[inboundindex].leftFace = INTERIOR;
          stateList.back().dependentstates[inboundindex].rightFace = EXTERIOR;
        }
        if(outswap)
        {
          stateList.back().dependentstates[outboundindex].leftFace = EXTERIOR;
          stateList.back().dependentstates[outboundindex].rightFace = INTERIOR;
        }
        else
        {
          stateList.back().dependentstates[outboundindex+1].leftFace = INTERIOR;
          stateList.back().dependentstates[outboundindex+1].rightFace = EXTERIOR;
        }
      }

      //Remove duplications
      sort(stateList.begin(), stateList.end());
      for(vector<VertexStateEdges>::iterator itr = stateList.begin();
        itr != stateList.end(); itr++)
      {
        vector<VertexStateEdges>::iterator next = itr+1;
        while(next != stateList.end() && (*itr) == (*next))
        {
          next = stateList.erase(next);
          std::cout << "DUPE Removed" << std::endl;
        }
      }
  }

  bool concaveTest(Edge *a, Edge *b)
  {
    float ax = a->edgestate.slope.den;
    float ay = a->edgestate.slope.num;
    float bx = b->edgestate.slope.den;
    float by = b->edgestate.slope.num;
    return( (ax*by - ay*bx) < 0.0f);
  }

  /* Takes in a list of vertices,
  * a list of valid states from source,
  * and an empty list to put state in,
  * and populates the empty list with
  * all possible valid states for all vertecies.
  */
  void generateStates(vector<Vertex*> &vertices,
    vector<VertexStateEdges> &sourceValidStates,
    vector<VertexState> &validStates, int *relativesCounter)
  {
    validStates.clear();
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
      Vertex *current = *(vertices.begin() + i);

      if(current->edges[0] == NULL || current->edges[1] == NULL ||
        current->edges[2] == NULL || current->edges[3] == NULL)
        //Force edge vertices to be empty
      {
        VertexStateEdges edge(0,0);
        edge.dependentstates[0].leftFace = EXTERIOR;
        edge.dependentstates[0].rightFace = EXTERIOR;
        edge.dependentstates[1].leftFace = EXTERIOR;
        edge.dependentstates[1].rightFace = EXTERIOR;
        edge.dependentstates[2].leftFace = EXTERIOR;
        edge.dependentstates[2].rightFace = EXTERIOR;
        edge.dependentstates[3].leftFace = EXTERIOR;
        edge.dependentstates[3].rightFace = EXTERIOR;
        validStates.push_back(VertexState(&(relativesCounter[i]), edge, current));
        relativesCounter[i]++;
        validStates.back().dependentedges[0] = current->edges[0];
        validStates.back().dependentedges[1] = current->edges[1];
        validStates.back().dependentedges[2] = current->edges[2];
        validStates.back().dependentedges[3] = current->edges[3];
        continue;
      }

      int sets[2];
      if(current->edges[0] != NULL)
        sets[0] = current->edges[0]->edgestate.set;
      else
        sets[0] = current->edges[1]->edgestate.set;
      if(current->edges[2] != NULL)
        sets[1] = current->edges[2]->edgestate.set;
      else
        sets[1] = current->edges[3]->edgestate.set;

      //For everything in possible vertex states list
      for(std::vector<VertexStateEdges>::iterator source_itr = sourceValidStates.begin();
        source_itr != sourceValidStates.end(); source_itr++)
      {
        if((*source_itr).setintersection[0] == sets[0] &&
          (*source_itr).setintersection[1] == sets[1] )
        {
          validStates.push_back(VertexState(&(relativesCounter[i]), (*source_itr), current));
          relativesCounter[i]++;
          validStates.back().dependentedges[0] = current->edges[0];
          validStates.back().dependentedges[1] = current->edges[1];
          validStates.back().dependentedges[2] = current->edges[2];
          validStates.back().dependentedges[3] = current->edges[3];
        }
      }
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
          (edge->edgestate.leftFace != (*itr).edgeinfo.dependentstates[i].leftFace ||
          edge->edgestate.rightFace != (*itr).edgeinfo.dependentstates[i].rightFace ))
        {
          //vertex is now an invalid vertex
          statevalid = false;
          continue;
        }
      }
      if (statevalid == false)
      {
        // Remove invalid vertex state from the vertexStates list
        (*((*itr).relativesCounter))--;
        itr = vertexStates.erase(itr);
      }
      else
        // Or continue
        itr++;
    }
  }

  VertexStateEdges::VertexStateEdges(int seta, int setb)
  {
    if(seta < setb)
    {
      setintersection[0] = seta;
      setintersection[1] = setb;
    }
    else
    {
      setintersection[0] = setb;
      setintersection[1] = seta;
    }
  }

  VertexStateEdges::VertexStateEdges(const VertexStateEdges &other)
  {
    setintersection[0] = other.setintersection[0];
    setintersection[1] = other.setintersection[1];
    dependentstates[0] = other.dependentstates[0];
    dependentstates[1] = other.dependentstates[1];
    dependentstates[2] = other.dependentstates[2];
    dependentstates[3] = other.dependentstates[3];
  }

  bool VertexStateEdges::operator<(VertexStateEdges &rhs)
  {
    // first check lower set
    if(setintersection[0] != rhs.setintersection[0])
    {
      if(setintersection[0] < rhs.setintersection[0])
        return true;
      else
        return false;
    }
    //then check upper set
    if(setintersection[1] != rhs.setintersection[1])
    {
      if(setintersection[1] < rhs.setintersection[1])
        return true;
      else
        return false;
    }
    
    //then check first edge
    if(dependentstates[0].leftFace != rhs.dependentstates[0].leftFace)
    {
      if(dependentstates[0].leftFace == EXTERIOR)
        return true;
      else
        return false;
    }
    if(dependentstates[0].rightFace != rhs.dependentstates[0].rightFace)
    {
      if(dependentstates[0].rightFace == EXTERIOR)
        return true;
      else
        return false;
    }
    //then check second edge
    if(dependentstates[1].leftFace != rhs.dependentstates[1].leftFace)
    {
      if(dependentstates[1].leftFace == EXTERIOR)
        return true;
      else
        return false;
    }
    if(dependentstates[1].rightFace != rhs.dependentstates[1].rightFace)
    {
      if(dependentstates[1].rightFace == EXTERIOR)
        return true;
      else
        return false;
    }
    //then check third edge
    if(dependentstates[2].leftFace != rhs.dependentstates[2].leftFace)
    {
      if(dependentstates[2].leftFace == EXTERIOR)
        return true;
      else
        return false;
    }
    if(dependentstates[2].rightFace != rhs.dependentstates[2].rightFace)
    {
      if(dependentstates[2].rightFace == EXTERIOR)
        return true;
      else
        return false;
    }
    //then check fourth edge
    if(dependentstates[3].leftFace != rhs.dependentstates[3].leftFace)
    {
      if(dependentstates[3].leftFace == EXTERIOR)
        return true;
      else
        return false;
    }
    if(dependentstates[3].rightFace != rhs.dependentstates[3].rightFace)
    {
      if(dependentstates[3].rightFace == EXTERIOR)
        return true;
      else
        return false;
    }
    //equals, with strict less than we return false
    return false;
  }

  bool VertexStateEdges::operator==(VertexStateEdges &rhs)
  {
    // first check lower set
    return !((setintersection[0] != rhs.setintersection[0])||
      //then check upper set
      (setintersection[1] != rhs.setintersection[1])||
      //then check first edge
      (dependentstates[0].leftFace != rhs.dependentstates[0].leftFace)||
      (dependentstates[0].rightFace != rhs.dependentstates[0].rightFace)||
      //then check second edge
      (dependentstates[1].leftFace != rhs.dependentstates[1].leftFace)||
      (dependentstates[1].rightFace != rhs.dependentstates[1].rightFace)||
      //then check third edge
      (dependentstates[2].leftFace != rhs.dependentstates[2].leftFace)||
      (dependentstates[2].rightFace != rhs.dependentstates[2].rightFace)||
      //then check fourth edge
      (dependentstates[3].leftFace != rhs.dependentstates[3].leftFace)||
      (dependentstates[3].rightFace != rhs.dependentstates[3].rightFace));
  }

  VertexState::VertexState(int *relCount, VertexStateEdges edges, Vertex *_parent):
  edgeinfo(edges)
  {
    parent = _parent;
    relativesCounter = relCount;
  }

  VertexState::VertexState(const VertexState &other):
  edgeinfo(other.edgeinfo)
  {
    dependentedges[0] = other.dependentedges[0];
    dependentedges[1] = other.dependentedges[1];
    dependentedges[2] = other.dependentedges[2];
    dependentedges[3] = other.dependentedges[3];
    relativesCounter = other.relativesCounter;
  }

  bool VertexState::operator<(VertexState &rhs)
  {
    if( *(relativesCounter) != *(rhs.relativesCounter) )
      return *(relativesCounter) < *(rhs.relativesCounter);
    return parent < rhs.parent;
  }
}