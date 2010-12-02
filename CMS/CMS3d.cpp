#include "CMS.h"
#include "CMS3d.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <time.h>
#include <limits>

namespace CMS3D
{
	int firstrun = true;
	BackTracker history;
   // std::vector<PotentialVertex*> unassignedVerticesSave;
	/* Continuous model synthesis main function
	*/
	bool continuousModelSynthesis(vector<Edge*> &edges, vector<Vertex*> &vertices,
		CMSModel3D &input,  Grid &grid)
	{
        Utils::Timer timer;
        cout << endl << "---Continuous Model Synthesis Progress---" << std::endl;
		unsigned int seed = (unsigned int)time(NULL);
        std::cout<< "Changing seed..." << seed << std::endl;
		srand(seed);
		if(firstrun)
		{
			for( std::vector<Vertex*>::iterator vertex_itr = vertices.begin();
				vertex_itr != vertices.end(); vertex_itr++)
			{
				sortEdges(*vertex_itr);
			}
            //firstrun = false;
		}

		PotentialVertex *currentVertex;
		std::vector<PotentialVertex*> unassignedVertices;
		std::vector<PotentialVertex*>::iterator unassignedItr;
		PotentialVertexState selectedState;

        if(firstrun)
        {
            cout << "Generating states..." << endl;
            timer.start();
		    bruteForceGenerate(unassignedVertices, input, grid);
            timer.stop();
            timer.printSeconds();
			firstrun = false;
		}
		else
		{
			while(history.selectedVertexList.size() > 0)
			{
				vector<vector<PotentialVertexState>*>::iterator listitr;
				vector<PotentialVertexState>::iterator stateitr;
				listitr = history.constrainedVertexList.back().begin();
				stateitr = history.removedStatesList.back().begin();

				while( listitr != history.constrainedVertexList.back().end() && stateitr != history.removedStatesList.back().end() )
				{
					(*listitr)->push_back(*stateitr);
					listitr++;
					stateitr++;
				}
				history.constrainedVertexList.pop_back();
				history.removedStatesList.pop_back();

				unassignedVertices.push_back(history.selectedVertexList.back());
				unassignedVertices.back()->states.push_back(history.selectedState.back());
				history.selectedVertexList.pop_back();
				history.selectedState.pop_back();
			}
		}

        cout << "Assigning states..." << endl;
        timer.start();
		while(unassignedVertices.size() > 0)
		{
			//std::cout << "Assignments Remaining: " << unassignedVertices.size() << std::endl;
			sort(unassignedVertices.begin(), unassignedVertices.end(), sortVertex);
			if( (*unassignedVertices.begin())->states.size() == 0)
			{
				//unassignedVertices.erase(unassignedVertices.begin());
				//continue;
				//backtrack
				//std::cout  << "BACKTRACK: " << unassignedVertices.size() << std::endl;
				vector<vector<PotentialVertexState>*>::iterator listitr;
				vector<PotentialVertexState>::iterator stateitr;
				listitr = history.constrainedVertexList.back().begin();
				stateitr = history.removedStatesList.back().begin();

				while( listitr != history.constrainedVertexList.back().end() && stateitr != history.removedStatesList.back().end() )
				{
					(*listitr)->push_back(*stateitr);
					listitr++;
					stateitr++;
				}
				history.constrainedVertexList.pop_back();
				history.removedStatesList.pop_back();
				if(history.constrainedVertexList.size() == 0 || history.removedStatesList.size() == 0)
				{
					std::cout << "!!!" << std::endl;
					break;
				}
				history.constrainedVertexList.back().push_back(&(history.selectedVertexList.back()->states));
				history.removedStatesList.back().push_back(history.selectedState.back());
				unassignedVertices.push_back(history.selectedVertexList.back());
				history.selectedVertexList.pop_back();
				history.selectedState.pop_back();
				continue;
			}
			unassignedItr = unassignedVertices.begin();
			currentVertex = *(unassignedItr);
			unassignedVertices.erase(unassignedItr);
			selectedState = currentVertex->getAndRemoveRandomState();
			history.selectedVertexList.push_back(currentVertex);
			history.selectedState.push_back(selectedState);
			history.constrainedVertexList.push_back(vector<vector<PotentialVertexState>*>());
			history.removedStatesList.push_back(vector<PotentialVertexState>());
			for(int i = 0; i < NUM_VOLUMES; i++)
			{
				if(currentVertex->volumes[i] != NULL)
				{
					constrainVolume(currentVertex->volumes[i], selectedState.volumes[i], unassignedVertices, history);
				}
			}
		}
		for(std::vector<PotentialVertex*>::iterator itr = unassignedVertices.begin(); itr != unassignedVertices.end(); itr++)
		{
			delete (*itr);
		}
        timer.stop();
        timer.printSeconds();
		return true;
	}

	/* 
	* sotrEdges( Vertex *v)
	* Sorts the edges in a vertex edge list based on
	* What faces created the edges.
	*/
	void sortEdges(Vertex *v)
	{
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
		for(int itr = 0; itr < v->connectedEdges; itr++)
		{
			Face* temp;
			if(v->faces[0]->set < v->faces[1]->set)
			{
				temp = v->faces[0];
				v->faces[0] = v->faces[1];
				v->faces[1] = temp;
			}
			if(v->faces[1]->set < v->faces[2]->set)
			{
				temp = v->faces[1];
				v->faces[1] = v->faces[2];
				v->faces[2] = temp;
			}
			if(v->faces[0]->set < v->faces[1]->set)
			{
				temp = v->faces[0];
				v->faces[0] = v->faces[1];
				v->faces[1] = temp;
			}

			//Find face sets connecting to edge, there should be 2;
			int face1;
			int face2;
			face1 = temps[itr]->faces[0]->set;
			for(int fitr = 1; temps[itr]->connectedFaces; fitr++)
			{
				if(temps[itr]->faces[fitr]->set != face1)
				{
					face2 = temps[itr]->faces[fitr]->set;
					break;
				}
			}

			if((face1 == v->faces[0]->set && face2 == v->faces[1]->set) ||
				(face1 == v->faces[1]->set && face2 == v->faces[0]->set))
			{
				if( temps[itr]->end == v)
					v->edges[SET1IN] = temps[itr];
				else
					v->edges[SET1OUT] = temps[itr];
			}
			else
			{
				if((face1 == v->faces[0]->set && face2 == v->faces[2]->set) ||
					(face1 == v->faces[2]->set && face2 == v->faces[0]->set))
				{
					if( temps[itr]->end == v)
						v->edges[SET2IN] = temps[itr];
					else
						v->edges[SET2OUT] = temps[itr];
				}
				else
				{
					if((face1 == v->faces[1]->set && face2 == v->faces[2]->set) ||
						(face1 == v->faces[2]->set && face2 == v->faces[1]->set))
					{
						if( temps[itr]->end == v)
							v->edges[SET3IN] = temps[itr];
						else
							v->edges[SET3OUT] = temps[itr];
					}
					else
					{
						std::cout << "Edge doesn't belong to any sets of faces!" << std::endl;
					}

				}
			}
		}
		if(v->connectedEdges == 6 && (v->edges[0] == NULL ||v->edges[1] == NULL ||v->edges[2] == NULL ||v->edges[3] == NULL ||
			v->edges[4] == NULL ||v->edges[5] == NULL) )
		{
			std::cout << "SUPERBAD" << std::endl;
		}
	}

	void bruteForceGenerate(vector<PotentialVertex*> &vertexList, CMSModel3D &input, Grid &grid)
	{
		for( unsigned int itr = 0; itr < grid.vertices.size(); itr++)
		{
			Vertex *sample = grid.vertices[itr];
			vertexList.push_back(new PotentialVertex());
			vertexList.back()->vertex = sample;
			vertexList.back()->findVolumes();

			//int height = 50;
			//if(vertexList.back()->vertex->val[0] > 20.0f &&
			//   vertexList.back()->vertex->val[0] < 30.0f &&
			//   vertexList.back()->vertex->val[2] < -25.0f &&
			//   vertexList.back()->vertex->val[2] > -30.0f &&
			//   vertexList.back()->vertex->val[1] < height )
			//{
			//  addInsideState(vertexList.back());
			//  continue;
			//}

			//if(vertexList.back()->vertex->connectedEdges != 6)
			//{
			//  addOutsideState(vertexList.back());
			//  continue;
			//}

			addOutsideState(vertexList.back());
			addInsideState(vertexList.back());

			//Translate edges to thier directions
			//IN edge vectors are reversed because the point in
			//We want thier vector pointing out
			if(sample->edges[SET1IN] != NULL)
			{
				vertexList.back()->edgeDirections[SET1IN] = Vector(
					sample->edges[SET1IN]->begin->val[0] - sample->edges[SET1IN]->end->val[0],
					sample->edges[SET1IN]->begin->val[1] - sample->edges[SET1IN]->end->val[1],
					sample->edges[SET1IN]->begin->val[2] - sample->edges[SET1IN]->end->val[2] );
			}
			if(sample->edges[SET1OUT] != NULL)
			{
				vertexList.back()->edgeDirections[SET1OUT] = Vector(
					sample->edges[SET1OUT]->end->val[0] - sample->edges[SET1OUT]->begin->val[0],
					sample->edges[SET1OUT]->end->val[1] - sample->edges[SET1OUT]->begin->val[1],
					sample->edges[SET1OUT]->end->val[2] - sample->edges[SET1OUT]->begin->val[2] );
			}
			if(sample->edges[SET2IN] != NULL)
			{
				vertexList.back()->edgeDirections[SET2IN] = Vector(
					sample->edges[SET2IN]->begin->val[0] - sample->edges[SET2IN]->end->val[0],
					sample->edges[SET2IN]->begin->val[1] - sample->edges[SET2IN]->end->val[1],
					sample->edges[SET2IN]->begin->val[2] - sample->edges[SET2IN]->end->val[2] );
			}
			if(sample->edges[SET2OUT] != NULL)
			{
				vertexList.back()->edgeDirections[SET2OUT] = Vector(
					sample->edges[SET2OUT]->end->val[0] - sample->edges[SET2OUT]->begin->val[0],
					sample->edges[SET2OUT]->end->val[1] - sample->edges[SET2OUT]->begin->val[1],
					sample->edges[SET2OUT]->end->val[2] - sample->edges[SET2OUT]->begin->val[2] );
			}
			if(sample->edges[SET3IN] != NULL)
			{
				vertexList.back()->edgeDirections[SET3IN] = Vector(
					sample->edges[SET3IN]->begin->val[0] - sample->edges[SET3IN]->end->val[0],
					sample->edges[SET3IN]->begin->val[1] - sample->edges[SET3IN]->end->val[1],
					sample->edges[SET3IN]->begin->val[2] - sample->edges[SET3IN]->end->val[2] );
			}
			if(sample->edges[SET3OUT] != NULL)
			{
				vertexList.back()->edgeDirections[SET3OUT] = Vector(
					sample->edges[SET3OUT]->end->val[0] - sample->edges[SET3OUT]->begin->val[0],
					sample->edges[SET3OUT]->end->val[1] - sample->edges[SET3OUT]->begin->val[1],
					sample->edges[SET3OUT]->end->val[2] - sample->edges[SET3OUT]->begin->val[2] );
			}
			//Normalize edge vectors
			for(int eitr = 0; eitr < NUM_EDGES; eitr++)
			{
				vertexList.back()->edgeDirections[eitr].normalize();
			}

			//For all elements in the input model check if the surrounding volumes are inside or outside
			for(int iitr = 0; iitr < input.numVertices; iitr++)
			{
				//need to check if vertex is valid with current vertex
				addVertexStates(vertexList.back(), input,
					Vector(input.vertices[iitr].val[0],
					input.vertices[iitr].val[1],
					input.vertices[iitr].val[2] ));
			}
			for(int iitr = 0; iitr < input.numFaces; iitr++)
			{
				for(int iiitr = 0; iiitr < input.faces[iitr].numEdges; iiitr++)
				{
					int next = iiitr+1;
					if(next == input.faces[iitr].numVertices) next = 0;
					Edge e;
					e.begin = input.faces[iitr].vertices[iiitr];
					e.end = input.faces[iitr].vertices[next];
					//need to check if edge is valid with current vertex
					addEdgeStates(vertexList.back(), input,  e);
				}
				//need to check if face is valid with current vertex
				addFaceStates(vertexList.back(), input, input.faces[iitr]);
			}
			//Done adding possible vertex states

			//Check all vertex states and remove duplicates for this vertex
			for(vector<PotentialVertexState>::iterator PVSitr = vertexList.back()->states.begin();
				PVSitr != vertexList.back()->states.end(); PVSitr++)
			{
				for(vector<PotentialVertexState>::iterator PVSitr2 = PVSitr+1;
					PVSitr2 != vertexList.back()->states.end();)
				{
					if( (*PVSitr) == (*PVSitr2) )
						PVSitr2 = vertexList.back()->states.erase(PVSitr2);
					else
						 PVSitr2++;
				}
			}

      //sanity check
			if(vertexList.back()->states.size() == 0)
			{
				std::cout << "PotentialVertex with 0 states";
			}
		}
	}

	void addInsideState(PotentialVertex *potential)
	{
		PotentialVertexState state;
		for(int itr = 0; itr < NUM_VOLUMES; itr++)
		{
			state.volumes[itr] = INTERIOR;
		}
		potential->states.push_back(state);
	}

	void addOutsideState(PotentialVertex *potential)
	{
		PotentialVertexState state;
		for(int itr = 0; itr < NUM_VOLUMES; itr++)
		{
			state.volumes[itr] = EXTERIOR;
		}
		potential->states.push_back(state);
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

	bool vertexInsideModel(CMSModel3D &input, Vector &vertex, Vector &trydir)
	{
		int testtrue = 0;
		Vector vector;
		int numintersections;

    //Snap points to ABOVE the ground (y < 0) on the input model. this prevents creating floating stuff
    //If the input model is not supposed to float then it's bottom vertices must have y values < 0.1
    if(vertex.y < 0.0f) vertex.y = 0.1f; //Slight offset above ground

		for(int testitr = 0; testitr < 15; testitr++)
		{
			numintersections = 0;
			vector.x = (float)((rand() % 10) + 1)/10.0f;
			vector.y = (float)((rand() % 10) + 1)/10.0f;
			vector.z = (float)((rand() % 10) + 1)/10.0f;
			vector.normalize();
			trydir.normalize();
			vector += trydir * 10;
			vector.normalize();
			for(int itr = 0; itr < input.numFaces; itr++)
			{
				if(vectorIntersectFace(input.faces[itr], vertex, vector))
					numintersections++;
			}
			if((numintersections % 2) == 1)
				testtrue++;
		}
		return (testtrue > 7);
	}

	void addVertexStates(PotentialVertex *potential, CMSModel3D &input, Vector &vert)
	{
		PotentialVertexState state;
		Vector offset;

		//IN_IN_IN
		if(potential->volumes[IN_IN_IN] != NULL)
		{
			offset =
				(potential->edgeDirections[SET1IN]) +
				(potential->edgeDirections[SET2IN]) +
				(potential->edgeDirections[SET3IN]);
			offset.normalize();
			state.volumes[IN_IN_IN] =
				vertexInsideModel(input, vert + (offset * 0.10f), offset) ? INTERIOR:EXTERIOR;
		}
		else
			state.volumes[IN_IN_IN] = EXTERIOR;
		//IN_IN_OUT
		if(potential->volumes[IN_IN_OUT] != NULL)
		{
			offset =
				(potential->edgeDirections[SET1IN]) +
				(potential->edgeDirections[SET2IN]) +
				(potential->edgeDirections[SET3OUT]);
			offset.normalize();
			state.volumes[IN_IN_OUT] =
				vertexInsideModel(input, vert + (offset * 0.10f), offset) ? INTERIOR:EXTERIOR;
		}
		else
			state.volumes[IN_IN_OUT] = EXTERIOR;
		//IN_OUT_IN
		if(potential->volumes[IN_OUT_IN] != NULL)
		{
			offset =
				(potential->edgeDirections[SET1IN]) +
				(potential->edgeDirections[SET2OUT]) +
				(potential->edgeDirections[SET3IN]);
			offset.normalize();
			state.volumes[IN_OUT_IN] =
				vertexInsideModel(input, vert + (offset * 0.10f), offset) ? INTERIOR:EXTERIOR;
		}
		else
			state.volumes[IN_OUT_IN] = EXTERIOR;
		//IN_OUT_OUT 
		if(potential->volumes[IN_OUT_OUT] != NULL)
		{
			offset =
				(potential->edgeDirections[SET1IN]) +
				(potential->edgeDirections[SET2OUT]) +
				(potential->edgeDirections[SET3OUT]);
			offset.normalize();
			state.volumes[IN_OUT_OUT] =
				vertexInsideModel(input, vert + (offset * 0.10f), offset) ? INTERIOR:EXTERIOR;
		}
		else
			state.volumes[IN_OUT_OUT] = EXTERIOR;
		//OUT_IN_IN
		if(potential->volumes[OUT_IN_IN] != NULL)
		{
			offset =
				(potential->edgeDirections[SET1OUT]) +
				(potential->edgeDirections[SET2IN]) +
				(potential->edgeDirections[SET3IN]);
			offset.normalize();
			state.volumes[OUT_IN_IN] =
				vertexInsideModel(input, vert + (offset * 0.10f), offset) ? INTERIOR:EXTERIOR;
		}
		else
			state.volumes[OUT_IN_IN] = EXTERIOR;
		//OUT_IN_OUT
		if(potential->volumes[OUT_IN_OUT] != NULL)
		{
			offset =
				(potential->edgeDirections[SET1OUT]) +
				(potential->edgeDirections[SET2IN]) +
				(potential->edgeDirections[SET3OUT]);
			offset.normalize();
			state.volumes[OUT_IN_OUT] =
				vertexInsideModel(input, vert + (offset * 0.10f), offset) ? INTERIOR:EXTERIOR;
		}
		else
			state.volumes[OUT_IN_OUT] = EXTERIOR;
		//OUT_OUT_IN
		if(potential->volumes[OUT_OUT_IN] != NULL)
		{
			offset =
				(potential->edgeDirections[SET1OUT]) +
				(potential->edgeDirections[SET2OUT]) +
				(potential->edgeDirections[SET3IN]);
			offset.normalize();
			state.volumes[OUT_OUT_IN] =
				vertexInsideModel(input, vert + (offset * 0.10f), offset) ? INTERIOR:EXTERIOR;
		}
		else
			state.volumes[OUT_OUT_IN] = EXTERIOR;
		//OUT_OUT_OUT
		if(potential->volumes[OUT_OUT_OUT] != NULL)
		{
			offset =
				(potential->edgeDirections[SET1OUT]) +
				(potential->edgeDirections[SET2OUT]) +
				(potential->edgeDirections[SET3OUT]);
			offset.normalize();
			state.volumes[OUT_OUT_OUT] =
				vertexInsideModel(input, vert + (offset * 0.10f), offset) ? INTERIOR:EXTERIOR;
		}
		else
			state.volumes[OUT_OUT_OUT] = EXTERIOR;

		//push state
		potential->states.push_back(state);
	}

	void addEdgeStates(PotentialVertex *potential,CMSModel3D &input, Edge &edge)
	{
		Vector vert;
		vert.x = (edge.begin->val[0] + edge.end->val[0])/2.0f;
		vert.y = (edge.begin->val[1] + edge.end->val[1])/2.0f;
		vert.z = (edge.begin->val[2] + edge.end->val[2])/2.0f;
		addVertexStates(potential, input, vert);
	}

	void addFaceStates(PotentialVertex *potential,CMSModel3D &input, Face &face)
	{
		Vector vert;
		vert.x = 0.0f;
		vert.y = 0.0f;
		vert.z = 0.0f;
		for(int x = 0; x < face.numVertices; x++)
		{
			vert.x += face.vertices[x]->val[0];
			vert.y += face.vertices[x]->val[1];
			vert.z += face.vertices[x]->val[2];
		}
		vert /= (float)face.numVertices;
		addVertexStates(potential, input, vert);
	}

	/* Take in an edge, the state parameters we will set it to 
	* and a list of all the valid vertexStates.
	* Sets the edge to the constrained state and removes
	* invalid vertex states that do not conform to this edge
	*/
	void constrainVolume(Volume *volume, VolumeState &state,
		vector<PotentialVertex*> &unassignedVertices, BackTracker &history)
	{
		//Change the edge to the contstrained state
		volume->state = state;
		//return;
		//For all vertex states
		for(vector<PotentialVertex*>::iterator itr = unassignedVertices.begin();
			itr != unassignedVertices.end(); itr++)
		{
			//For all volumes in the vertex states
			for(int i = 0; i < NUM_VOLUMES; i++)
			{
				//see if the volume is the volume we are constraining
				if((*itr)->volumes[i] == volume)
				{
					//For all potential states
					for(vector<PotentialVertexState>::iterator state_itr = (*itr)->states.begin();
						state_itr != (*itr)->states.end();)
					{
						// If potential states state is not compatible with constraint
						if( (*state_itr).volumes[i] != state )
						{
							history.constrainedVertexList.back().push_back(&((*itr)->states));
							history.removedStatesList.back().push_back(*state_itr);
							state_itr = (*itr)->states.erase(state_itr);
							continue;
						}
						state_itr++;
					}
				}
			}
		}
	}

	PotentialVertexState::PotentialVertexState(const PotentialVertexState &rhs)
	{
		for(int itr = 0; itr < NUM_VOLUMES; itr++)
		{
			volumes[itr] = rhs.volumes[itr];
		}
	}

	PotentialVertexState& PotentialVertexState::operator=(const PotentialVertexState &rhs)
	{
		for(int itr = 0; itr < NUM_VOLUMES; itr++)
		{
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
		states = other.states;
		for(int itr = 0; itr < NUM_VOLUMES; itr++)
			volumes[itr] = other.volumes[itr];
		for(int itr = 0; itr < NUM_EDGES; itr++)
			edgeDirections[itr] = other.edgeDirections[itr];
		for(int itr = 0; itr < NUM_FACE_SETS; itr++)
			sets[itr] = other.sets[itr];
	}

	bool PotentialVertex::operator<(const PotentialVertex &lhs) const
	{
		if(states.size() < lhs.states.size()) return true;
		if(states.size() == lhs.states.size())
			return vertex->val[1] < lhs.vertex->val[1];
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
					if(ea->faces[a]->leftVolume != NULL && 
						(ea->faces[a]->leftVolume == eb->faces[b]->leftVolume || ea->faces[a]->leftVolume == eb->faces[b]->rightVolume ) && 
						(ea->faces[a]->leftVolume == ec->faces[c]->leftVolume || ea->faces[a]->leftVolume == ec->faces[c]->rightVolume ))
					{
						return ea->faces[a]->leftVolume;
					}
					if(ea->faces[a]->rightVolume != NULL &&
						(ea->faces[a]->rightVolume == eb->faces[b]->leftVolume || ea->faces[a]->rightVolume == eb->faces[b]->rightVolume ) && 
						(ea->faces[a]->rightVolume == ec->faces[c]->leftVolume || ea->faces[a]->rightVolume == ec->faces[c]->rightVolume ))
					{
						return ea->faces[a]->rightVolume;
					}
				}
			}
		}
		std::cout << "WTF THIS SUCKS" << std::endl;
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

	PotentialVertexState PotentialVertex::getAndRemoveRandomState()
	{
		PotentialVertexState state;
		vector<PotentialVertexState>::iterator itr = states.begin();
		int random = rand() % states.size();
		//random = 0;
		itr +=  random;
		state = *itr;
		states.erase(itr);
		return state;
	}

	
  bool sortVertex (PotentialVertex* lhs, PotentialVertex* rhs)
  {
	  return lhs->operator<(*rhs);
  }


  BackTracker::~BackTracker()
  {
	  for(vector<PotentialVertex*>::iterator itr = selectedVertexList.begin(); itr != selectedVertexList.end(); itr++)
	  {
		  delete (*itr);
	  }
  }
}