#ifndef _CMS3D_H
#define _CMS3D_H

#include "CMSModel3D.h"
#include "Geometry.h"
#include "Grid.h"

namespace CMS3D
{
	enum VertexEdgeIndex {SET1IN = 0, SET1OUT, SET2IN, SET2OUT, SET3IN, SET3OUT};
	enum VertexSetIndex {SET1 = 0, SET2 = 1, SET3 = 2};
	enum VertexDefenition {NUM_EDGES = 6, NUM_FACE_SETS = 3, NUM_VOLUMES = 8};
  enum VertexVolumes{IN_IN_IN = 0, IN_IN_OUT = 1, IN_OUT_IN = 2, IN_OUT_OUT = 3, OUT_IN_IN = 4, OUT_IN_OUT = 5, OUT_OUT_IN = 6, OUT_OUT_OUT = 7};

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
    VolumeState volumes[NUM_VOLUMES];

		PotentialVertexState(){};
		PotentialVertexState(const PotentialVertexState &rhs);
		PotentialVertexState& operator=(const PotentialVertexState &rhs);
		bool operator==(const PotentialVertexState &rhs) const;
	};

	class PotentialVertex
	{
	public:
		int sets[NUM_FACE_SETS];
		Vertex *vertex;
    Volume *volumes[NUM_VOLUMES];
		vector<PotentialVertexState> states;
    Vector edgeDirections[NUM_EDGES];
    
		PotentialVertex(){};
    void findVolumes();
    PotentialVertexState getRandomState();
    bool operator<(const PotentialVertex &lhs) const;
  };

  bool continuousModelSynthesis2D(vector<Edge*> &edges, vector<Vertex*> &vertices,
    CMSModel3D &input, Grid &grid);
  bool concaveTest(Edge *a, Edge *b);
  void sortEdges(Vertex *v);

  //The one real function
  void generateStates(vector<PotentialVertex> &stateList,  CMSModel3D &input, Grid &grid);
  void addInsideState(PotentialVertex &potential);
  void addOutsideState(PotentialVertex &potential);
  void addVertexStates(PotentialVertex &potential, CMSModel3D &input, Grid &grid, Vertex &vertex);
  void addEdgeStates(PotentialVertex &potential,CMSModel3D &input, Grid &grid, Edge &edge);
  void addFaceStates(PotentialVertex &potential,CMSModel3D &input, Grid &grid, Face &face);


  void assignStates(vector<Vertex*> &vertices, vector<PotentialVertex> &source,
    vector<PotentialVertex> &stateList);
  void constrainVolume(Volume *volume, VolumeState &state,
    vector<PotentialVertex> &potentialVertices);
}
#endif