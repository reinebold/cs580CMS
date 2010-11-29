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
	bool continuousModelSynthesis(vector<Edge*> &edges, vector<Vertex*> &vertices,
		CMSModel3D &input,  Grid &grid)
	{
		for( std::vector<Vertex*>::iterator vertex_itr = vertices.begin();
			vertex_itr != vertices.end(); vertex_itr++)
			sortEdges(*vertex_itr);

		std::vector<PotentialVertex> validStates;
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
			for(int i = 0; i < NUM_VOLUMES; i++)
			{
				if(assignedVertices.back().volumes[i] != NULL)
				{
          constrainVolume(assignedVertices.back().volumes[i],
            selectedState.volumes[i], validVertices);
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
	* What faces created the edges.
	*/
	void sortEdges(Vertex *v)
	{
    //Unrolled buble sort of 3 face sets
    Face *ftemp;
    if(v->faces[0]->set > v->faces[1]->set)
    {
      ftemp = v->faces[0];
      v->faces[0] = v->faces[1];
      v->faces[1] = ftemp;
    }
    if(v->faces[1]->set > v->faces[2]->set)
    {
      ftemp = v->faces[1];
      v->faces[1] = v->faces[2];
      v->faces[2] = ftemp;
    }
    if(v->faces[0]->set > v->faces[1]->set)
    {
      ftemp = v->faces[0];
      v->faces[0] = v->faces[1];
      v->faces[1] = ftemp;
    }

    //copy edges to the temp array;
    Edge *temps[NUM_EDGES];
    for(int itr = 0 ; itr < NUM_EDGES; itr++)
    {
      if(itr < v->connectedEdges)
        temps[itr] = v->edges[itr];
      else
        temps[itr] = NULL;
      v->edges[itr] = NULL;
		}

    //For all edges in temp array move them back based on thier direction
    //and faces they intersect with
		for(int itr = 0; itr < NUM_EDGES; itr++)
		{
      if(itr >=  v->connectedEdges)
      {
        continue;
      }

      //Find face sets connecting to edge, there should be 2;
      int face1;
      int face2;
      face1 = temps[itr]->faces[0]->set;
      for(int fitr = 1; fitr < 4/*temps[itr]->connectedFaces*/; fitr++)
      {
        if(temps[itr]->faces[fitr]->set != face1)
        {
          face2 = temps[itr]->faces[fitr]->set;
          break;
        }
      }
      if(face1 > face2)
      {
        face1=face2;
        face2 = temps[itr]->faces[0]->set;
      }

      if(face1 == v->faces[0]->set && face2 == v->faces[1]->set)
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
      {
				if(face1 == v->faces[0]->set && face2 == v->faces[2]->set)
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
        {
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
		}
	}

	/* Populates stateList with a list of valid states
	*/
	void generateStates(vector<PotentialVertex> &vertexList,  CMSModel3D &input,
		Grid &grid)
	{
		//find the faces that connect to this vertex
		for( int itr = 0; itr < input.numVertices; itr++)
		{
			int faces[3];
			int numFaces = 0;
			std::vector<PotentialVertex>::iterator PVSitr;
			PVSitr = vertexList.begin();
			vector<vector<Face*>>::iterator PFitr;
      numFaces = 0;
      for(PFitr = grid.parallelFaces.begin(); PFitr != grid.parallelFaces.end(); PFitr++)
			{
				for(int y = 0; y < 3; y++)
				{
					if(((*(*PFitr).begin())->normal) == (input.vertices[itr].faces[y]->normal) || (
						(fabs(((*(*PFitr).begin())->normal).x - -(input.vertices[itr].faces[y]->normal).x) < EPSILON) &&
						(fabs(((*(*PFitr).begin())->normal).y - -(input.vertices[itr].faces[y]->normal).y) < EPSILON) &&
						(fabs(((*(*PFitr).begin())->normal).z - -(input.vertices[itr].faces[y]->normal).z) < EPSILON) ))
					{
						faces[numFaces]=(*(*PFitr).begin())->set;
						numFaces++;
            break;
          }
        }
        if( numFaces == 3) break;
      }
      //Sanity Check
      if(numFaces > 3)
      {
        std::cout << "Error Vertex matches too many faces\n";
      }
			//done finding faces connected to vertex

      //Check if potential vertex is being duplicated
			for(PVSitr = vertexList.begin(); PVSitr != vertexList.end(); PVSitr++)
			{
				if(	faces[0] == PVSitr->sets[0] &&
					faces[1] == PVSitr->sets[1] &&
          faces[2] == PVSitr->sets[2] ){break;}
			}

      //No duplication found, make a new vertex
			if(PVSitr == vertexList.end())
			{
				vertexList.push_back(PotentialVertex());
				vertexList.back().sets[0] = faces[0];
				vertexList.back().sets[1] = faces[1];
				vertexList.back().sets[2] = faces[2];
				vertexList.back().vertex = NULL;

        addInsideState(vertexList.back());
        addOutsideState(vertexList.back());
        
        //Give the new potential vertex edges based on the input faces
        Vertex *sample;
        //find sample vertex in grid to obtain edge directions from
        for(vector<Vertex*>::iterator sitr = grid.vertices.begin();
          sitr != grid.vertices.end(); sitr++)
        {
          sample = (*sitr);
          if(sample->faces[0]->set == faces[0] &&
             sample->faces[1]->set == faces[1] &&
             sample->faces[2]->set == faces[2] &&
             sample->connectedEdges == 6 )
          {
            break;
          }
        }
        //Translate edges to thier directions
        //IN edge vectors are reversed because the point in
        //We want thier vector pointing out
        vertexList.back().edgeDirections[SET1IN] = Vector(
          sample->edges[SET1IN]->begin->val[0] - sample->edges[SET1IN]->end->val[0],
          sample->edges[SET1IN]->begin->val[1] - sample->edges[SET1IN]->end->val[1],
          sample->edges[SET1IN]->begin->val[2] - sample->edges[SET1IN]->end->val[2] );
        vertexList.back().edgeDirections[SET1OUT] = Vector(
          sample->edges[SET1OUT]->end->val[0] - sample->edges[SET1OUT]->begin->val[0],
          sample->edges[SET1OUT]->end->val[1] - sample->edges[SET1OUT]->begin->val[1],
          sample->edges[SET1OUT]->end->val[2] - sample->edges[SET1OUT]->begin->val[2] );
        vertexList.back().edgeDirections[SET2IN] = Vector(
          sample->edges[SET2IN]->begin->val[0] - sample->edges[SET2IN]->end->val[0],
          sample->edges[SET2IN]->begin->val[1] - sample->edges[SET2IN]->end->val[1],
          sample->edges[SET2IN]->begin->val[2] - sample->edges[SET2IN]->end->val[2] );
        vertexList.back().edgeDirections[SET2OUT] = Vector(
          sample->edges[SET2OUT]->end->val[0] - sample->edges[SET2OUT]->begin->val[0],
          sample->edges[SET2OUT]->end->val[1] - sample->edges[SET2OUT]->begin->val[1],
          sample->edges[SET2OUT]->end->val[2] - sample->edges[SET2OUT]->begin->val[2] );
        vertexList.back().edgeDirections[SET3IN] = Vector(
          sample->edges[SET3IN]->begin->val[0] - sample->edges[SET3IN]->end->val[0],
          sample->edges[SET3IN]->begin->val[1] - sample->edges[SET3IN]->end->val[1],
          sample->edges[SET3IN]->begin->val[2] - sample->edges[SET3IN]->end->val[2] );
        vertexList.back().edgeDirections[SET3OUT] = Vector(
          sample->edges[SET3OUT]->end->val[0] - sample->edges[SET3OUT]->begin->val[0],
          sample->edges[SET3OUT]->end->val[1] - sample->edges[SET3OUT]->begin->val[1],
          sample->edges[SET3OUT]->end->val[2] - sample->edges[SET3OUT]->begin->val[2] );
        //Normalize edge vectors
        for(int eitr = 0; eitr < NUM_EDGES; eitr++)
        {
          vertexList.back().edgeDirections[eitr].normalize();
        }

        //For all elements in the input model check if the surrounding volumes are inside or outside
				for(int iitr = 0; iitr < input.numVertices; iitr++)
				{
					addVertexStates(vertexList.back(), input, grid, input.vertices[iitr]);
				}
				for(int iitr = 0; iitr < input.numFaces; iitr++)
				{
					for(int iiitr = 0; iiitr < input.faces[iitr].numEdges; iiitr++)
					{
						addEdgeStates(vertexList.back(), input, grid, *(input.faces[iitr].edges[iiitr]));
					}
					addFaceStates(vertexList.back(), input, grid, input.faces[iitr]);
				}
        //Done adding possible vertex states

        //Check all vertex states and remove duplicates for this vertex
        for(vector<PotentialVertexState>::iterator PVSitr = vertexList.back().states.begin();
          PVSitr != vertexList.back().states.end(); PVSitr++)
        {
          for(vector<PotentialVertexState>::iterator PVSitr2 = PVSitr+1;
            PVSitr2 != vertexList.back().states.end(); PVSitr2++)
          {
            if( (*PVSitr) == (*PVSitr2) )
              PVSitr2 = vertexList.back().states.erase(PVSitr2);
          }
        }
        //sanity check
        if(vertexList.back().states.size() == 0)
        {
          std::cout << "PotentialVertex with 0 states";
        }
			}
		}
	}

  void addInsideState(PotentialVertex &potential)
  {
    PotentialVertexState state;
    for(int itr = 0; itr < NUM_VOLUMES; itr++)
    {
      state.volumes[itr] = INTERIOR;
    }
    potential.states.push_back(state);
  }

  void addOutsideState(PotentialVertex &potential)
  {
    PotentialVertexState state;
    for(int itr = 0; itr < NUM_VOLUMES; itr++)
    {
      state.volumes[itr] = EXTERIOR;
    }
    potential.states.push_back(state);
  }

  bool vectorIntersectFace(Face &face, Vector &vertex, Vector &vector)
  {
    float distancetoplane;
    Vector pointOnPlane;
    //check ditance to face plane
    pointOnPlane.x = face.vertices[0]->val[0];
    pointOnPlane.y = face.vertices[0]->val[1];
    pointOnPlane.z = face.vertices[0]->val[2];
    distancetoplane = face.normal.dotProduct( pointOnPlane - vertex )/face.normal.dotProduct(vector);
    if(!(distancetoplane > 0))
      return false;
    pointOnPlane = vertex + (vector * distancetoplane);
    // Continue check if point on plane is inside of polygonal face
    // Assumes polygonal face is convex in counter clockwise ordering
    // So that cross product between edge and line from edge to intersection
    // Point should always be similar to normal if point is inside
    for(int itr = 0; itr < face.numVertices; itr++)
    {
      int next = itr+1;
      if(next == face.numVertices) next = 0;
      Vector a = Vector(
        face.vertices[next]->val[0] - face.vertices[itr]->val[0],
        face.vertices[next]->val[1] - face.vertices[itr]->val[1],
        face.vertices[next]->val[2] - face.vertices[itr]->val[2]);
      Vector b = Vector(
        pointOnPlane.x - face.vertices[itr]->val[0],
        pointOnPlane.y - face.vertices[itr]->val[1],
        pointOnPlane.z - face.vertices[itr]->val[2]);
      Vector cross = a.crossProduct(b);
      cross.normalize();
      //check if the cross of a and b resembles normal
      //and return false if it doesn't
      if(cross.dotProduct(face.normal) < 0.0f)
        return false;
    }
    return true;
  }

  bool vertexInsideModel(CMSModel3D &input, Vector &vertex)
  {
    int testtrue = 0;
    Vector vert;
    Vector vector;
    int numintersections;

    for(int testitr = 0; testitr < 3; testitr++)
    {
      numintersections = 0;
      vector.x = ((rand() % 10) + 1)/10.0f;
      vector.y = ((rand() % 10) + 1)/10.0f;
      vector.z = ((rand() % 10) + 1)/10.0f;
      vector.normalize();
      for(int itr = 0; itr < input.numFaces; itr++)
      {
        if(vectorIntersectFace(input.faces[itr], vertex, vector))
          numintersections++;
      }
      if((numintersections % 2) == 1)
        testtrue++;
    }
    return (testtrue > 1);
  }

	void addVertexStates(PotentialVertex &potential, CMSModel3D &input, Grid &grid, Vertex &vertex)
	{
    PotentialVertexState state;
    Vector vert;
    Vector offset;

    vert.x = vertex.val[0];
    vert.y = vertex.val[1];
    vert.z = vertex.val[2];

    //IN_IN_IN
    offset =
      (potential.edgeDirections[SET1IN]) +
      (potential.edgeDirections[SET2IN]) +
      (potential.edgeDirections[SET3IN]);
    offset.normalize();
    state.volumes[IN_IN_IN] =
      vertexInsideModel(input, vert + (offset * 0.10f)) ? INTERIOR:EXTERIOR;
    //IN_IN_OUT
    offset =
      (potential.edgeDirections[SET1IN]) +
      (potential.edgeDirections[SET2IN]) +
      (potential.edgeDirections[SET3OUT]);
    offset.normalize();
    state.volumes[IN_IN_OUT] =
      vertexInsideModel(input, vert + (offset * 0.10f)) ? INTERIOR:EXTERIOR;
    //IN_OUT_IN
    offset =
      (potential.edgeDirections[SET1IN]) +
      (potential.edgeDirections[SET2OUT]) +
      (potential.edgeDirections[SET3IN]);
    offset.normalize();
    state.volumes[IN_OUT_IN] =
      vertexInsideModel(input, vert + (offset * 0.10f)) ? INTERIOR:EXTERIOR;
    //IN_OUT_OUT 
    offset =
      (potential.edgeDirections[SET1IN]) +
      (potential.edgeDirections[SET2OUT]) +
      (potential.edgeDirections[SET3OUT]);
    offset.normalize();
    state.volumes[IN_OUT_OUT] =
      vertexInsideModel(input, vert + (offset * 0.10f)) ? INTERIOR:EXTERIOR;
    //OUT_IN_IN
    offset =
      (potential.edgeDirections[SET1OUT]) +
      (potential.edgeDirections[SET2IN]) +
      (potential.edgeDirections[SET3IN]);
    offset.normalize();
    state.volumes[OUT_IN_IN] =
      vertexInsideModel(input, vert + (offset * 0.10f)) ? INTERIOR:EXTERIOR;
    //OUT_IN_OUT
    offset =
      (potential.edgeDirections[SET1OUT]) +
      (potential.edgeDirections[SET2IN]) +
      (potential.edgeDirections[SET3OUT]);
    offset.normalize();
    state.volumes[OUT_IN_OUT] =
      vertexInsideModel(input, vert + (offset * 0.10f)) ? INTERIOR:EXTERIOR;
    //OUT_OUT_IN
    offset =
      (potential.edgeDirections[SET1OUT]) +
      (potential.edgeDirections[SET2OUT]) +
      (potential.edgeDirections[SET3IN]);
    offset.normalize();
    state.volumes[OUT_OUT_IN] =
      vertexInsideModel(input, vert + (offset * 0.10f)) ? INTERIOR:EXTERIOR;
    //OUT_OUT_OUT
    offset =
      (potential.edgeDirections[SET1OUT]) +
      (potential.edgeDirections[SET2OUT]) +
      (potential.edgeDirections[SET3OUT]);
    offset.normalize();
    state.volumes[OUT_OUT_OUT] =
      vertexInsideModel(input, vert + (offset * 0.10f)) ? INTERIOR:EXTERIOR;

    //push state
    potential.states.push_back(state);
	}

	void addEdgeStates(PotentialVertex &potential,CMSModel3D &input, Grid &grid, Edge &edge)
	{
    Vertex vert;
    vert.val[0] = (edge.begin->val[0] + edge.end->val[0])/2.0f;
    vert.val[1] = (edge.begin->val[1] + edge.end->val[1])/2.0f;
    vert.val[2] = (edge.begin->val[2] + edge.end->val[2])/2.0f;
    addVertexStates(potential, input, grid, vert);
	}

	void addFaceStates(PotentialVertex &potential,CMSModel3D &input, Grid &grid, Face &face)
	{
    Vertex vert;
    vert.val[0] = 0.0f;
    vert.val[1] = 0.0f;
    vert.val[2] = 0.0f;
    for(int x = 0; x < face.numEdges; x++)
    {
      vert.val[0] += face.edges[x]->begin->val[0];
      vert.val[1] += face.edges[x]->begin->val[1];
      vert.val[2] += face.edges[x]->begin->val[2];
    }
    vert.val[0] /= (float)face.numEdges;
    vert.val[1] /= (float)face.numEdges;
    vert.val[2] /= (float)face.numEdges;
    addVertexStates(potential, input, grid, vert);
	}

	/* Takes in a list of vertices,
	* a list of valid states from source,
	* and an empty list to put state in,
	* and populates the empty list with
	* all possible valid states for all vertecies.
	*/
	void assignStates(vector<Vertex*> &vertices,
		vector<PotentialVertex> &sourceStates,
		vector<PotentialVertex> &validStates)
	{
		validStates.clear();
		for(unsigned int i = 0; i < vertices.size(); i++)
		{
			Vertex *current = *(vertices.begin() + i);
			validStates.push_back(PotentialVertex());
			validStates.back().vertex = current;
      validStates.back().findVolumes();

			if(current->edges[0] == NULL || current->edges[1] == NULL ||
				current->edges[2] == NULL || current->edges[3] == NULL ||
				current->edges[4] == NULL || current->edges[5] == NULL)
				//Force edge vertices to be empty
			{
				PotentialVertexState vertexState;
				for(int x = 0; x < NUM_VOLUMES; x++)
				{
          vertexState.volumes[x] = EXTERIOR;
				}
				validStates.back().states.push_back(vertexState);
				continue;
			}

			//For everything in possible vertex states list
			for(vector<PotentialVertex>::iterator PVitr = sourceStates.begin();
				PVitr != sourceStates.end(); PVitr++)
			{					
        if( (*PVitr).sets[SET1] == current->faces[SET1]->set &&
            (*PVitr).sets[SET2] == current->faces[SET2]->set &&
            (*PVitr).sets[SET3] == current->faces[SET3]->set  )
				{
					for(std::vector<PotentialVertexState>::iterator source_itr = sourceStates.back().states.begin();
						source_itr != sourceStates.back().states.end(); source_itr++)
					{
						validStates.back().states.push_back( *source_itr );
					}
				}
			}
		}
	}

	/* Take in an edge, the state parameters we will set it to 
	* and a list of all the valid vertexStates.
	* Sets the edge to the constrained state and removes
	* invalid vertex states that do not conform to this edge
	*/
  void constrainVolume(Volume *volume, VolumeState &state,
    vector<PotentialVertex> &potentialVertices)
	{
		//Change the edge to the contstrained state
    volume->state = state;

		//For all vertex states
		for(vector<PotentialVertex>::iterator itr = potentialVertices.begin();
			itr != potentialVertices.end(); itr++)
		{
			for(vector<PotentialVertexState>::iterator state_itr = (*itr).states.begin();
				state_itr != (*itr).states.end(); itr++)
			{
				//For all volumes in the vertex states
				for(int i = 0; i < NUM_VOLUMES; i++)
				{
					//see if the volume is the volume we are constraining
          if((*itr).volumes[i] == volume)
					{
						// If so, then see if th volume does not conform to the constrained state
            if(!((*state_itr).volumes[i] == volume->state))
						{
              if((*itr).states.size() == 1)
              {
                std::cout << "Error Couldn't Constrain A vertex Any More\n";
                continue;
              }
              //It it doesn't, delete it
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
    for(int itr = 0; itr < NUM_VOLUMES; itr++)
    {
      volumes[itr] = rhs.volumes[itr];
      volumes[itr] = rhs.volumes[itr];
    }
    return *this;
	}

	bool PotentialVertexState::operator==(const PotentialVertexState &lhs) const
	{
		for(int itr = 0; itr < NUM_VOLUMES; itr++)
		{
			if(!(volumes[itr] == lhs.volumes[itr]))
				return false;
		}
		return true;
	}

  PotentialVertex::PotentialVertex(const PotentialVertex &other)
  {
    vertex = other.vertex;
    for(int itr = 0; itr < NUM_VOLUMES; itr++)
      volumes[itr] = other.volumes[itr];
    states = other.states;
    for(int itr = 0; itr < NUM_EDGES; itr++)
      edgeDirections[itr] = other.edgeDirections[itr];
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

  Volume *findCommonVolume(Edge *ea, Edge *eb, Edge *ec)
  {
    if(ea == NULL || eb == NULL || ec == NULL) return NULL;
    for(int a = 0; a < ea->connectedFaces; a++)
    {
      for(int b = 0; b < eb->connectedFaces; b++)
      {
        for(int c = 0; c < ec->connectedFaces; c++)
        {
          if( (ea->faces[a]->leftVolume == eb->faces[b]->leftVolume && ea->faces[a]->leftVolume == ec->faces[c]->leftVolume) ||
              (ea->faces[a]->leftVolume == eb->faces[b]->leftVolume && ea->faces[a]->leftVolume == ec->faces[c]->rightVolume) ||
              (ea->faces[a]->leftVolume == eb->faces[b]->rightVolume && ea->faces[a]->leftVolume == ec->faces[c]->leftVolume) ||
              (ea->faces[a]->leftVolume == eb->faces[b]->rightVolume && ea->faces[a]->leftVolume == ec->faces[c]->rightVolume) )
              return ea->faces[a]->leftVolume;

          if( (ea->faces[a]->rightVolume == eb->faces[b]->leftVolume && ea->faces[a]->rightVolume == ec->faces[c]->leftVolume) ||
              (ea->faces[a]->rightVolume == eb->faces[b]->leftVolume && ea->faces[a]->rightVolume == ec->faces[c]->rightVolume) ||
              (ea->faces[a]->rightVolume == eb->faces[b]->rightVolume && ea->faces[a]->rightVolume == ec->faces[c]->leftVolume) ||
              (ea->faces[a]->rightVolume == eb->faces[b]->rightVolume && ea->faces[a]->rightVolume == ec->faces[c]->rightVolume) )
              return ea->faces[a]->rightVolume;
        }
      }
    }
    return NULL;
  }

  void PotentialVertex::findVolumes()
  {
    if(vertex == NULL) return;
    volumes[IN_IN_IN] = findCommonVolume(vertex->edges[SET1IN],vertex->edges[SET2IN],vertex->edges[SET3IN]);
    volumes[IN_IN_OUT] = findCommonVolume(vertex->edges[SET1IN],vertex->edges[SET2IN],vertex->edges[SET3OUT]);
    volumes[IN_OUT_IN] = findCommonVolume(vertex->edges[SET1IN],vertex->edges[SET2OUT],vertex->edges[SET3IN]);
    volumes[IN_OUT_OUT] = findCommonVolume(vertex->edges[SET1IN],vertex->edges[SET2OUT],vertex->edges[SET3OUT]);
    volumes[OUT_IN_IN] = findCommonVolume(vertex->edges[SET1OUT],vertex->edges[SET2IN],vertex->edges[SET3IN]);
    volumes[OUT_IN_OUT] = findCommonVolume(vertex->edges[SET1OUT],vertex->edges[SET2IN],vertex->edges[SET3OUT]);
    volumes[OUT_OUT_IN] = findCommonVolume(vertex->edges[SET1OUT],vertex->edges[SET2OUT],vertex->edges[SET3IN]);
    volumes[OUT_OUT_OUT] = findCommonVolume(vertex->edges[SET1OUT],vertex->edges[SET2OUT],vertex->edges[SET3OUT]);
  }

  PotentialVertexState PotentialVertex::getRandomState()
	{
		vector<PotentialVertexState>::iterator itr = states.begin();
		itr +=  rand() % states.size();
		return *itr;
	}
}