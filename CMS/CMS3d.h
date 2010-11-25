#ifndef _CMS3D_H
#define _CMS3D_H

#include "CMSModel.h"
#include "Geometry.h"
#include "Grid.h"

namespace CMS3D
{
	enum VertexEdgeIndex {SET1IN = 0, SET1OUT, SET2IN, SET2OUT, SET3IN, SET3OUT};
	enum VertexSetIndex {SET1 = 0, SET2 = 1, SET3 = 2};
	enum VertexDefenition {NUM_EDGES = 6, NUM_EDGE_SETS = 3};

	class PotentialFaceState
	{
	public:
		VolumeState leftVolume;
		VolumeState rightVolume;
	};

	class PotentialEdgeState
	{
	public:
		PotentialFaceState potentialFaces[4];

    PotentialEdgeState(){};
    PotentialEdgeState(const PotentialEdgeState &rhs);
    PotentialEdgeState& operator=(const PotentialEdgeState &rhs);
    bool operator==(const PotentialEdgeState &rhs) const;
    bool operator==(const Edge &rhs) const;
	};

	class PotentialVertexState
	{
	public:
		int sets[NUM_EDGE_SETS];
		PotentialEdgeState potentialEdges[NUM_EDGES];

		PotentialVertexState(){};
		PotentialVertexState(const PotentialVertexState &rhs);
		PotentialVertexState& operator=(const PotentialVertexState &rhs);
		bool operator==(const PotentialVertexState &rhs) const;
	};

	class PotentialVertex
	{
	public:
		PotentialVertex(){};
		Vertex *vertex;
		vector<PotentialVertexState> states;
		PotentialVertexState getRandomState();
		bool operator<(const PotentialVertex &lhs) const;
	};

	bool continuousModelSynthesis2D(vector<Edge*> &edges, vector<Vertex*> &vertices,
		CMSModel &input, Grid &grid);
	bool concaveTest(Edge *a, Edge *b);
	void sortEdges(Vertex *v);

	//The one real function
	void generateStates(vector<PotentialVertexState> &stateList,  CMSModel &input, Grid &grid);

	void assignStates(vector<Vertex*> &vertices,
		vector<PotentialVertexState> &source,
		vector<PotentialVertex> &stateList);
	void constrainEdge(Edge *edge, PotentialEdgeState &state,
		vector<PotentialVertex> &potentialVertices);
}
#endif