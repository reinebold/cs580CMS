

#include "Geometry.h"
using namespace Geometry;

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

IntersectResult Edge::intersect(const Edge& otheredge, Vertex *intersection)
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
        intersection->val[X] = begin->val[X] + ua*(end->val[X] - begin->val[X]);
        intersection->val[Y] = begin->val[Y] + ua*(end->val[Y] - begin->val[Y]);

        return INTERESECTING;
    }

    return NOT_INTERESECTING;
}