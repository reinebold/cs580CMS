#include "CMS3d.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <time.h>
#include <limits>

namespace CMS3D
{
	/* Continuous model synthesis main function
	*/
	bool continuousModelSynthesis2D(vector<Edge*> &edges, vector<Vertex*> &vertices,
		CMSModel &input,  Grid &grid)
	{
		for( std::vector<Vertex*>::iterator vertex_itr = vertices.begin();
			vertex_itr != vertices.end(); vertex_itr++)
			sortEdges(*vertex_itr);

		std::vector<PotentialVertexState> validStates;
		std::vector<PotentialVertex> validVertices;
		std::vector<PotentialVertex> assignedVertices;
		generateStates(validStates, input, grid);
		assignStates( vertices, validStates, validVertices);
		while(validVertices.size() > 0)
		{
			sort(validVertices.begin(), validVertices.end());
			if((*(validVertices.begin())).states.size() < 0)
				return false;
			std::vector<PotentialVertex>::iterator itr = validVertices.begin();
			PotentialVertexState selectedState;
			assignedVertices.push_back(*(validVertices.begin()));
			validVertices.erase(validVertices.begin());
			selectedState = assignedVertices.back().getRandomState();
			for(int i = 0; i < NUM_EDGES; i++)
			{
				if(assignedVertices.back().vertex->edges[i] != NULL)
				{
					constrainEdge(assignedVertices.back().vertex->edges[i],
						selectedState.potentialEdges[i], validVertices);
				}
			}
		}
		return true;
	}

	bool concaveTest(Edge *a, Edge *b)
	{
		float ax = a->edgestate.slope.den;
		float ay = a->edgestate.slope.num;
		float bx = b->edgestate.slope.den;
		float by = b->edgestate.slope.num;
		return( (ax*by - ay*bx) < 0.0f);
	}

	/* 
	* sotrEdges( Vertex *v)
	* Sorts the edges in a vertex edge list based on
	* Lower set first, then by
	* inbound edge (vertex is edges end) first
	*/
	void sortEdges(Vertex *v)
	{
		Edge *temps[NUM_EDGES];
		int sets[NUM_EDGE_SETS];
		sets[SET1] = INT_MAX;
		sets[SET3] = 0;
		for(int itr = 0 ; itr < NUM_EDGES; itr++)
		{
			temps[itr] = v->edges[itr];
			if(temps[itr]!=NULL)
				if(temps[itr]->edgestate.set < sets[SET1])
					sets[SET1] = temps[itr]->edgestate.set;
			if(temps[itr]->edgestate.set > sets[SET3])
				sets[SET3] = temps[itr]->edgestate.set;
		}

		for(int itr = 0 ; itr < NUM_EDGES; itr++)
		{
			if( temps[itr]->edgestate.set != sets[SET1] &&
				temps[itr]->edgestate.set != sets[SET3] )
			{
				sets[SET2] = temps[itr]->edgestate.set;
				break;
			}
		}

		for(int itr = 0; itr < NUM_EDGES; itr++)
		{
			if(temps[itr] == NULL) continue;
			if(temps[itr]->edgestate.set == sets[SET1])
			{
				if(temps[itr]->end == v)
				{
					v->edges[SET1IN] = temps[itr];
				}
				else
				{
					v->edges[SET1OUT] = temps[itr];
				}
			}
			else
				if(temps[itr]->edgestate.set == sets[SET2])
				{
					if(temps[itr]->end == v)
					{
						v->edges[SET2IN] = temps[itr];
					}
					else
					{
						v->edges[SET2OUT] = temps[itr];
					}
				}
				else
					if(temps[itr]->end == v)
					{
						v->edges[SET3IN] = temps[itr];
					}
					else
					{
						v->edges[SET3OUT] = temps[itr];
					}
		}
	}

	/* Populates stateList with a list of valid states
	* currently creates a pre-defined list
	* but latter will take in a model that describes possible states
	*/
	void generateStates(vector<PotentialVertexState> &stateList,  CMSModel &input,
		Grid &grid)
		//inputmodel is a counterclockwise list of vertices
	{
	}

	/* Takes in a list of vertices,
	* a list of valid states from source,
	* and an empty list to put state in,
	* and populates the empty list with
	* all possible valid states for all vertecies.
	*/
	void assignStates(vector<Vertex*> &vertices,
		vector<PotentialVertexState> &sourceStates,
		vector<PotentialVertex> &validStates)
	{
		validStates.clear();
		for(unsigned int i = 0; i < vertices.size(); i++)
		{
			Vertex *current = *(vertices.begin() + i);

			if(current->edges[0] == NULL || current->edges[1] == NULL ||
				current->edges[2] == NULL || current->edges[3] == NULL ||
				current->edges[4] == NULL || current->edges[5] == NULL)
				//Force edge vertices to be empty
			{
        PotentialVertexState edge;
        edge.sets[0] = 0;
        edge.sets[1] = 0;
        edge.sets[2] = 0;
        for(int x = 0; x < 6; x++)
        {
          edge.potentialEdges[0].potentialFaces[FACE1_LEFT].leftVolume = EXTERIOR;
          edge.potentialEdges[0].potentialFaces[FACE1_LEFT].rightVolume = EXTERIOR;
          edge.potentialEdges[0].potentialFaces[FACE1_RIGHT].leftVolume = EXTERIOR;
          edge.potentialEdges[0].potentialFaces[FACE1_RIGHT].rightVolume = EXTERIOR;
          edge.potentialEdges[0].potentialFaces[FACE2_LEFT].leftVolume = EXTERIOR;
          edge.potentialEdges[0].potentialFaces[FACE2_LEFT].rightVolume = EXTERIOR;
          edge.potentialEdges[0].potentialFaces[FACE2_RIGHT].leftVolume = EXTERIOR;
          edge.potentialEdges[0].potentialFaces[FACE2_RIGHT].rightVolume = EXTERIOR;
        }
        validStates.push_back(PotentialVertex());
        validStates.back().vertex = current;
        validStates.back().states.push_back(edge);
        continue;
      }

			int sets[NUM_EDGE_SETS];
			if(current->edges[SET1IN] != NULL)
				sets[SET1] = current->edges[SET1IN]->edgestate.set;
			else
				sets[SET1] = current->edges[SET1OUT]->edgestate.set;
			if(current->edges[SET2IN] != NULL)
				sets[SET2] = current->edges[SET2IN]->edgestate.set;
			else
				sets[SET2] = current->edges[SET2OUT]->edgestate.set;
			if(current->edges[SET3IN] != NULL)
				sets[SET3] = current->edges[SET3IN]->edgestate.set;
			else
				sets[SET3] = current->edges[SET3OUT]->edgestate.set;

			//For everything in possible vertex states list
			for(std::vector<PotentialVertexState>::iterator source_itr = sourceStates.begin();
				source_itr != sourceStates.end(); source_itr++)
			{
				if( (*source_itr).sets[SET1] == sets[SET1] &&
					(*source_itr).sets[SET2] == sets[SET2] &&
					(*source_itr).sets[SET3] == sets[SET3] )
				{
					validStates.push_back(PotentialVertex());
					validStates.back().vertex = current;
					validStates.back().states.push_back( *source_itr );
				}
			}
		}
	}

	/* Take in an edge, the state parameters we will set it to 
	* and a list of all the valid vertexStates.
	* Sets the edge to the constrained state and removes
	* invalid vertex states that do not conform to this edge
	*/
	void constrainEdge(Edge *edge, PotentialEdgeState &state,
		vector<PotentialVertex> &vertexStates)
	{
		//Change the edge to the contstrained state
    edge->faces[FACE1_LEFT]->leftVolume->state = state.potentialFaces[FACE1_LEFT].leftVolume;
		edge->faces[FACE1_LEFT]->rightVolume->state = state.potentialFaces[FACE1_LEFT].rightVolume;
		edge->faces[FACE1_RIGHT]->leftVolume->state = state.potentialFaces[FACE1_RIGHT].leftVolume;
		edge->faces[FACE1_RIGHT]->rightVolume->state = state.potentialFaces[FACE1_RIGHT].rightVolume;
		edge->faces[FACE2_LEFT]->leftVolume->state = state.potentialFaces[FACE2_LEFT].leftVolume;
		edge->faces[FACE2_LEFT]->rightVolume->state = state.potentialFaces[FACE2_LEFT].rightVolume;
		edge->faces[FACE2_RIGHT]->leftVolume->state = state.potentialFaces[FACE2_RIGHT].leftVolume;
		edge->faces[FACE2_RIGHT]->rightVolume->state = state.potentialFaces[FACE2_RIGHT].rightVolume;


		//For all vertex states
		for(vector<PotentialVertex>::iterator itr = vertexStates.begin();
			itr != vertexStates.end(); itr++)
		{
			for(vector<PotentialVertexState>::iterator state_itr = (*itr).states.begin();
				state_itr != (*itr).states.end(); itr++)
			{
				//For all edges in the vertex states
				for(int i = 0; i < NUM_EDGES; i++)
				{
					//see if the edge is the edge we are concidering
					if((*itr).vertex->edges[i] == edge)
					{
						// If so, then see if th edge does not conform to the constrained state
						if(!((*state_itr).potentialEdges[i] == *edge))
						{
							state_itr = (*itr).states.erase(state_itr);
						}
					}
				}
			}
		}
  }

  PotentialEdgeState::PotentialEdgeState(const PotentialEdgeState &rhs)
  {
    *this = rhs;
  }

  PotentialEdgeState& PotentialEdgeState::operator=(const PotentialEdgeState &rhs)
  {
    potentialFaces[FACE1_LEFT].leftVolume = rhs.potentialFaces[FACE1_LEFT].leftVolume;
    potentialFaces[FACE1_LEFT].rightVolume = rhs.potentialFaces[FACE1_LEFT].rightVolume;
    potentialFaces[FACE1_RIGHT].leftVolume = rhs.potentialFaces[FACE1_RIGHT].leftVolume;
    potentialFaces[FACE1_RIGHT].rightVolume = rhs.potentialFaces[FACE1_RIGHT].rightVolume;
    potentialFaces[FACE2_LEFT].leftVolume = rhs.potentialFaces[FACE2_LEFT].leftVolume;
    potentialFaces[FACE2_LEFT].rightVolume = rhs.potentialFaces[FACE2_LEFT].rightVolume;
    potentialFaces[FACE2_RIGHT].leftVolume = rhs.potentialFaces[FACE2_RIGHT].leftVolume;
    potentialFaces[FACE2_RIGHT].rightVolume = rhs.potentialFaces[FACE2_RIGHT].rightVolume;
    return *this;
  }

  bool PotentialEdgeState::operator==(const PotentialEdgeState &rhs) const
  {
    return (
      rhs.potentialFaces[FACE1_LEFT].leftVolume == potentialFaces[FACE1_LEFT].leftVolume &&
      rhs.potentialFaces[FACE1_LEFT].rightVolume == potentialFaces[FACE1_LEFT].rightVolume &&
      rhs.potentialFaces[FACE1_RIGHT].leftVolume == potentialFaces[FACE1_RIGHT].leftVolume &&
      rhs.potentialFaces[FACE1_RIGHT].rightVolume == potentialFaces[FACE1_RIGHT].rightVolume &&
      rhs.potentialFaces[FACE2_LEFT].leftVolume == potentialFaces[FACE2_LEFT].leftVolume &&
      rhs.potentialFaces[FACE2_LEFT].rightVolume == potentialFaces[FACE2_LEFT].rightVolume &&
      rhs.potentialFaces[FACE2_RIGHT].leftVolume == potentialFaces[FACE2_RIGHT].leftVolume &&
      rhs.potentialFaces[FACE2_RIGHT].rightVolume == potentialFaces[FACE2_RIGHT].rightVolume);

  }

  bool PotentialEdgeState::operator==(const Edge &rhs) const
  {
    return (
      rhs.faces[FACE1_LEFT]->leftVolume->state == potentialFaces[FACE1_LEFT].leftVolume &&
      rhs.faces[FACE1_LEFT]->rightVolume->state == potentialFaces[FACE1_LEFT].rightVolume &&
      rhs.faces[FACE1_RIGHT]->leftVolume->state == potentialFaces[FACE1_RIGHT].leftVolume &&
      rhs.faces[FACE1_RIGHT]->rightVolume->state == potentialFaces[FACE1_RIGHT].rightVolume &&
      rhs.faces[FACE2_LEFT]->leftVolume->state == potentialFaces[FACE2_LEFT].leftVolume &&
      rhs.faces[FACE2_LEFT]->rightVolume->state == potentialFaces[FACE2_LEFT].rightVolume &&
      rhs.faces[FACE2_RIGHT]->leftVolume->state == potentialFaces[FACE2_RIGHT].leftVolume &&
      rhs.faces[FACE2_RIGHT]->rightVolume->state == potentialFaces[FACE2_RIGHT].rightVolume);
  }

	PotentialVertexState::PotentialVertexState(const PotentialVertexState &rhs)
	{
		*this = rhs;
	}

	PotentialVertexState& PotentialVertexState::operator=(const PotentialVertexState &rhs)
	{
		for(int itr = 0; itr < NUM_EDGE_SETS; itr++)
		{
			sets[itr] = rhs.sets[itr];
		}
		for(int itr = 0; itr < NUM_EDGES; itr++)
		{
      for(int it2 = 0; it2 < 4; it2++)
      {
			potentialEdges[itr].potentialFaces[it2].leftVolume =
        rhs.potentialEdges[itr].potentialFaces[it2].leftVolume;
			potentialEdges[itr].potentialFaces[it2].rightVolume =
        rhs.potentialEdges[itr].potentialFaces[it2].rightVolume;
      }
		}
		return *this;
	}

	bool PotentialVertexState::operator==(const PotentialVertexState &lhs) const
	{
		for(int itr = 0; itr < NUM_EDGE_SETS; itr++)
		{
			if(!(sets[itr] == lhs.sets[itr]))
				return false;
		}
		for(int itr = 0; itr < NUM_EDGES; itr++)
		{
			if(!(potentialEdges[itr]== lhs.potentialEdges[itr]))
				return false;
		}
		return true;
	}

	bool PotentialVertex::operator<(const PotentialVertex &lhs) const
	{
		if(states.size() < lhs.states.size()) return true;
		if(states.size() > lhs.states.size()) return false;
		if(vertex->val[0] < lhs.vertex->val[0]) return true;
		if(vertex->val[0] > lhs.vertex->val[0]) return false;
		if(vertex->val[1] < lhs.vertex->val[1]) return true;
		if(vertex->val[1] > lhs.vertex->val[1]) return false;
		if(vertex->val[2] < lhs.vertex->val[2]) return true;
		if(vertex->val[2] > lhs.vertex->val[2]) return false;
		if(vertex->val[3] < lhs.vertex->val[3]) return true;
		if(vertex->val[3] > lhs.vertex->val[3]) return false;
		return false;
	}

	PotentialVertexState PotentialVertex::getRandomState()
	{
		vector<PotentialVertexState>::iterator itr = states.begin();
		itr +=  rand() % states.size();
		return *itr;
	}
}