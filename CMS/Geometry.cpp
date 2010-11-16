
#include "Defines.h"
#include "Geometry.h"
#include <math.h>
using namespace Geometry;

EdgeState::EdgeState()
    :leftFace(UNASSIGNED),
     rightFace(UNASSIGNED),
     set(-1)
{
}

Fraction::Fraction()
:num(0),
 den(0),
 value(0)
{

}

Fraction::Fraction(float numval, float denval)
    :num(numval),
     den(denval)
{
    if(den != 0)
    {
        value = num/den;
    }
}

bool Fraction::operator==(const Fraction &rhs) const
{
    return ((fabs(num-rhs.num) < EPSILON) && (fabs(den-rhs.den) < EPSILON));
}

void Fraction::operator=(const Fraction &rhs)
{
    num = rhs.num;
    den = rhs.den;
    value = rhs.value;
}

Vertex::Vertex()
    :connectedEdges(0)
{
    val[X] = 0.0f;
    val[Y] = 0.0f;
    val[Z] = 0.0f;

    edges[0] = NULL;
    edges[1] = NULL;
    edges[2] = NULL;
    edges[3] = NULL;
}

Vertex::Vertex(float xval, float yval, float zval)
    :connectedEdges(0)
{
    val[X] = xval;
    val[Y] = yval;
    val[Z] = zval;

    edges[0] = NULL;
    edges[1] = NULL;
    edges[2] = NULL;
    edges[3] = NULL;
}

void Vertex::operator=(const Vertex &vert)
{
    val[X] = vert.val[X];
    val[Y] = vert.val[Y];
    val[Z] = vert.val[Z];

    connectedEdges = vert.connectedEdges;
    edges[0] = vert.edges[0];
    edges[1] = vert.edges[1];
    edges[2] = vert.edges[2];
    edges[3] = vert.edges[3];
}

Edge::Edge() 
    :begin(NULL),
     end(NULL)
{

}

Edge::Edge(Vertex* beginval, Vertex* endval)
    :begin(beginval),
     end(endval)
{
    updateEdgeState();
}

void Edge::operator=(const Edge &edge)
{
    begin = edge.begin;
    end = edge.end;

    updateEdgeState();
}

void Edge::updateEdgeState()
{
    edgestate.slope.num = (end->val[Y] - begin->val[Y]);
    edgestate.slope.den = (end->val[X] - begin->val[X]);
    edgestate.slope.value = edgestate.slope.num/edgestate.slope.den;
    edgestate.leftFace  = UNASSIGNED;
    edgestate.rightFace = UNASSIGNED;
}

Cuboid::Cuboid()
{

}

void Cuboid::init(Vertex *_verticies)
{
    //Assign the verticies to the data member verticies in Cuboid.
    for(int x = 0; x < 4; ++x)
    {
        verticies[x] = _verticies[x];
    }

    //Assign the edges (based on the verticies)
    for(int x = 0; x < 4; ++x)
    {
        int begin = x;
        int end;
        if(x+1 == 4)
        {
            end = 0;
        }
        else
        {
            end = x+1;
        }

        edges[x].begin = &verticies[begin];
        edges[x].end = &verticies[end];

        edges[x].edgestate.slope.num = (verticies[end].val[Y] - verticies[begin].val[Y]);
        edges[x].edgestate.slope.den = (verticies[end].val[X] - verticies[begin].val[X]);
        edges[x].edgestate.slope.value = edges[x].edgestate.slope.num/edges[x].edgestate.slope.den;
        edges[x].edgestate.leftFace = UNASSIGNED;
        edges[x].edgestate.rightFace = UNASSIGNED;
    }
}

/// Modified from http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/ (Author: Damian Coventry)
IntersectResult Edge::intersect(const Edge& otheredge, Vertex* &intersection)
{
    float denom  = ((otheredge.end->val[Y] - otheredge.begin->val[Y])*(end->val[X] - begin->val[X])) -
                   ((otheredge.end->val[X] - otheredge.begin->val[X])*(end->val[Y] - begin->val[Y]));

    float nume_a = ((otheredge.end->val[X] - otheredge.begin->val[X])*(begin->val[Y] - otheredge.begin->val[Y])) -
                   ((otheredge.end->val[Y] - otheredge.begin->val[Y])*(begin->val[X] - otheredge.begin->val[X]));

    float nume_b = ((end->val[X] - begin->val[X])*(begin->val[Y] - otheredge.begin->val[Y])) -
                   ((end->val[Y] - begin->val[Y])*(begin->val[X] - otheredge.begin->val[X]));

    if(denom == 0.0f)
    {
        if(nume_a == 0.0f && nume_b == 0.0f)
        {
            return COINCIDENT;
        }
        return PARALLEL;
    }

    float ua = nume_a / denom;
    float ub = nume_b / denom;

    if(ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
    {
        // Get the intersection point.
        intersection = new Vertex();
        intersection->val[X] = begin->val[X] + ua*(end->val[X] - begin->val[X]);
        intersection->val[Y] = begin->val[Y] + ua*(end->val[Y] - begin->val[Y]);

        return INTERESECTING;
    }

    return NOT_INTERESECTING;
}