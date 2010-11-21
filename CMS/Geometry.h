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
        Vertex(float xval, float yval, float zval = 0.0f);

        void operator=(const Vertex &vert);

        float val[3];          ///< X, Y, and Z values
        Edge *edges[4];        ///< The maximum 4 edges the vertex is connected to
        int   connectedEdges;  ///< Number of edges the vertex has.
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
        
        void init(int numVerticies, Vertex *verticies);

        //TODO: Update cuboid to 3d case
        Vertex  verticies[4];   ///< Bounding box verticies
        Edge    edges[4];       ///< Bounding box edges
    };


	class Vector {
	public:
		Vector();
		Vector(float x, float y, float z);
		float dotProduct(Vector& other);
		Vector crossProduct(Vector& other);
		float getX();
		float getY();
		float getZ();
		void setX(float newX);
		void setY(float newY);
		void setZ(float newZ);
		Vector& operator=(const Vector& other);
	private:
		float x, y, z;
	};

	class Plane {
	public:
		Plane(Vertex v, Vector dir);
	private:
		Vertex v;
		Vector dir;
	};

	class Face
    {
    public:
        Face(int numVertices, Vertex* vertices);
		Vertex* getVertices();
		Edge* getEdges();
		Vector getNormal();
		~Face();
	private:
		int numVertices;
		int numEdges;
		Vertex* vertices;
		Edge* edges;
		Vector normal;
    };
}

#endif // _GEOMETRY_H