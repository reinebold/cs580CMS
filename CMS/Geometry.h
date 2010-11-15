/**
    \file Geometry
    \brief 
*/

#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "Defines.h"
#include <math.h>
#define EPSILON .0001
#define INFINITY FLT_MAX

namespace Geometry
{
    enum IntersectResult {PARALLEL = 0, COINCIDENT, NOT_INTERESECTING, INTERESECTING};
    enum FaceState       {EXTERIOR = 0, INTERIOR = 1, UNASSIGNED = -1};
    enum EdgePosition    {BOTTOM   = 0, RIGHT, TOP, LEFT};

    class Edge;

    //TODO: Make more robust...
    class Fraction
    {
    public:
        Fraction() {}
        Fraction(float numval, float denval)
            :num(numval),
             den(denval)
        {
            if(den != 0)
            {
                value = num/den;
            }
        }

        bool operator==(Fraction &fraction)
        {
            return ((fabs(num-fraction.num) < EPSILON) && (fabs(den-fraction.den) < EPSILON));
        }

        void operator=(const Fraction &fraction)
        {
            num = fraction.num;
            den = fraction.den;
            value = fraction.value;
        }

        float num;
        float den;
        float value;
    };

    class Vertex
    {
    public:
        Vertex()
            :currentEdge(0)
        {
            val[X] = 0.0f;
            val[Y] = 0.0f;
            val[Z] = 0.0f;

            edges[0] = NULL;
            edges[1] = NULL;
            edges[2] = NULL;
            edges[3] = NULL;
        }

        Vertex(float xval, float yval, float zval = 0.0f)
            :currentEdge(0)
        {
            val[X] = xval;
            val[Y] = yval;
            val[Z] = zval;

            edges[0] = NULL;
            edges[1] = NULL;
            edges[2] = NULL;
            edges[3] = NULL;
        }

        void operator=(Vertex &vert)
        {
            val[X] = vert.val[X];
            val[Y] = vert.val[Y];
            val[Z] = vert.val[Z];

            currentEdge = vert.currentEdge;
            edges[0] = vert.edges[0];
            edges[1] = vert.edges[1];
            edges[2] = vert.edges[2];
            edges[3] = vert.edges[3];
        }

        float val[3];

        Edge *edges[4];
        int currentEdge;
    };

    class EdgeState
    {
    public:
        EdgeState() {}

        FaceState leftFace;
        FaceState rightFace;
        Fraction slope;
        int set;
    };

    class Edge
    {
    public:
        Vertex* begin;
        Vertex* end;

        Edge() {}

        Edge(Vertex* beginval, Vertex* endval)
            :begin(beginval),
             end(endval)
        {
            edgestate.slope.num = (end->val[Y] - begin->val[Y]);
            edgestate.slope.den = (end->val[X] - begin->val[X]);
            edgestate.slope.value = edgestate.slope.num/edgestate.slope.den;
            edgestate.leftFace  = UNASSIGNED;
            edgestate.rightFace = UNASSIGNED;
        }

        void operator=(const Edge &edge)
        {
            begin = edge.begin;
            end = edge.end;

            edgestate.slope.num = (end->val[Y] - begin->val[Y]);
            edgestate.slope.den = (end->val[X] - begin->val[X]);
            edgestate.slope.value = edgestate.slope.num/edgestate.slope.den;
            edgestate.leftFace  = UNASSIGNED;
            edgestate.rightFace = UNASSIGNED;
        }

        IntersectResult intersect(const Edge& otheredge, Vertex* &intersection);

        EdgeState edgestate;
    };

    class Cuboid
    {
    public:
        Cuboid() {}
        
        //TODO: Right now it only works with 4 verticies...a square.
        void init(Vertex *verticies);

        Vertex  verticies[4];
        Edge    edges[4];
    };
}

#endif // _GEOMETRY_H