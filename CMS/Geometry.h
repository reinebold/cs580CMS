/**
    \file  Geometry.h
    \brief 
*/

#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "Defines.h"
#include <math.h>        ///< For fabs
#define EPSILON .0001    ///< For use when comparing floats.
#define INFINITY FLT_MAX ///< To signify a vertical line

namespace Geometry
{
    enum IntersectResult {PARALLEL = 0, COINCIDENT, NOT_INTERESECTING, INTERESECTING};
    enum FaceState       {EXTERIOR = 0, INTERIOR = 1, UNASSIGNED = -1};
    enum EdgePosition    {BOTTOM   = 0, RIGHT, TOP, LEFT};

    class Edge;
    class Face;

    class Fraction
    {
    public:
        Fraction();
        Fraction(float numval, float denval);

        bool operator==(const Fraction &rhs) const;
        void operator=(const Fraction &rhs);

        float num;      ///< Numerator
        float den;      ///< Denominator
        float value;    ///< Numerator/Denominator
    };

    class Vertex
    {
    public:
        Vertex();
        Vertex(float xval, float yval, float zval = 0.0f, float wval = 1.0f);

        Vertex& operator=(const Vertex &vert);
        bool operator==(const Vertex &vert);

        float val[4];          ///< X, Y, Z, W values
        Edge *edges[4];        ///< The maximum 4 edges the vertex is connected to
        int   connectedEdges;  ///< Number of edges the vertex has.
        int   connectedFaces;  ///< Number of faces the vertex has.
        Face *faces[4];        ///< Number of faces the vertex has been intersected by
    };

    class EdgeState
    {
    public:
        EdgeState();

        FaceState leftFace;     ///< Keeps track of the face to the left of the edge
        FaceState rightFace;    ///< Keeps track of the face to the right of the edge
        Fraction  slope;        ///< Slope of the edge
        int       set;          ///< Keeps track of what set of parallel edges it belongs to
    };

    class Edge
    {
    public:
        Edge();
        Edge(Vertex* beginval, Vertex* endval);

        void operator=(const Edge &edge);
        void updateEdgeState();                        ///< Calulates the numerator, denominator, and value of slope
        IntersectResult intersect(const Edge& otheredge, Vertex* &intersection);  ///< Returns intersection vertex in given parameter

        Vertex*   begin;        ///< Begin vertex (counter-clockwise direction)
        Vertex*   end;          ///< End vertex (counter-clockwise direction)
        EdgeState edgestate;    ///< Keeps track of facestates, slope, and set
    };

    class Cuboid
    {
    public:
        Cuboid();
        
        void init(int _numVertices, Vertex *vertices);

        //TODO: Update cuboid to 3d case
		int	 	 numVertices;
        int      numFaces;
        Vertex  *vertices;   ///< Bounding box vertices
        Edge    *edges;       ///< Bounding box edges
        Face    *faces;
    };

	class Vector {
	public:
		Vector();
		Vector(float x, float y, float z);
		float dotProduct(Vector& other);
		Vector crossProduct(Vector& other);
		Vector& operator=(const Vector& other);
        bool operator==(const Vector& rhs);

		float x, y, z;
	};

	class Plane {
	public:
		Plane(Vertex v, Vector dir);

		Vertex v;
		Vector dir;
	};

	class Face
    {
    public:
        Face();
        Face(int _numVertices, Vertex** _vertices);
		~Face();
        Face& operator=(const Face &_face);
        void updateFaces();

		int numVertices;
		int numEdges;
		Vertex** vertices;  //array of Vertex*
		Edge* edges;
		Vector normal;
    };

	/*Returns a new (dynamically allocated) Edge (with the two endpoints).*/
    Edge* planeFaceIntersection(const Plane &plane, const Face &face); 

    //Takes in four edges. Returns new (dynamically allocated Face)
    Face* createFace(Edge *edge, const int numEdges) ;

    //Returns vertex of the intersection.  Make sure you fill the 'faces' data member that points to the faces that intersected it so we can find it's normals.
    Vertex* faceFaceFaceIntersection(const Face &face1, const Face &face2, const Face &face3);

	//Tells you if vertex is in sphere.  Simple I think....return (vert.val[X]-center.val[X])^2+(vert.val[Y] - center.val[Y])^2+(vert.val[Z] - center.val[Z])^2 <= radius^2
	bool vertexInSphere(Vertex *center, Vertex *vert, float radius);
}

#endif // _GEOMETRY_H