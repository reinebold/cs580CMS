
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

Vertex& Vertex::operator=(const Vertex &vert)
{
    val[X] = vert.val[X];
    val[Y] = vert.val[Y];
    val[Z] = vert.val[Z];

    connectedEdges = vert.connectedEdges;
    edges[0] = vert.edges[0];
    edges[1] = vert.edges[1];
    edges[2] = vert.edges[2];
    edges[3] = vert.edges[3];
	return *this;
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

void Cuboid::init(int _numVerticies, Vertex *_verticies)
{
	if(_numVerticies != 8 && _numVerticies != 4)
	{
		cerr << "Bounding box can only have 4 (for 2d case) or 8 (for 3d case) verticies. numVerticies: " << numVerticies << endl;
		exit(EXIT_FAILURE);
	}

    numVerticies = _numVerticies;

    verticies = new Vertex[numVerticies];
    edges = new Edge[numVerticies];

    //Assign the verticies to the data member verticies in Cuboid.
    for(int x = 0; x < numVerticies; ++x)
    {
        verticies[x] = _verticies[x];
    }

    //Assign the edges (based on the verticies)
    for(int x = 0; x < numVerticies; ++x)
    {
        int begin = x;
        int end;
        if(x+1 == numVerticies)
        {
            end = 0;
        }
        else
        {
            end = x+1;
        }

        edges[x].begin = &verticies[begin];
        edges[x].end = &verticies[end];

        edges[x].updateEdgeState();
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


Vector::Vector(float inX, float inY, float inZ) {
	x = inX;
	y = inY;
	z = inZ;
}

Vector::Vector() {
	x = 0;
	y = 0;
	z = 0;
}

float Vector::getX() {
	return x;
}

float Vector::getY() {
	return y;
}

float Vector::getZ() {
	return z;
}

void Vector::setX(float newX) {
	x = newX;
}

void Vector::setY(float newY) {
	y = newY;
}

void Vector::setZ(float newZ) {
	z = newZ;
}

float Vector::dotProduct(Vector& other) {
	return x * other.x + y * other.y + z * other.z;
}

Vector& Vector::operator=(const Vector& other) {
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

Vector Vector::crossProduct(Vector& other) {
	Vector answer(0, 0, 0);
	answer.x = (y * other.z) - (z * other.y);
	answer.y = (z * other.x) - (x * other.z);
	answer.z = (x * other.y) - (y * other.x);
	return answer;
}

Plane::Plane(Vertex vIn, Vector dirIn) {
	v = vIn;
	dir = dirIn;
}

Face::Face(){

}

Face::Face(int inNumVertices, Vertex* inVertices) {
	numVertices = inNumVertices;
	numEdges = inNumVertices;
	vertices = inVertices;
	edges = new Edge[numEdges];
	for(int i=0; i < numVertices; i++) {
		Vertex v1 = vertices[i];
		Vertex v2;
		if (i == numVertices - 1) {
			v2 = vertices[0];
		} else {
			v2 = vertices[i + 1];
		}
		Edge e(&v1, &v2);
		edges[i] = e;
	}
	Vector a(edges[0].end->val[0] - edges[0].begin->val[0], edges[0].end->val[1] - edges[0].begin->val[1], edges[0].end->val[2] - edges[0].begin->val[2]);
	Vector b(edges[1].end->val[0] - edges[1].begin->val[0], edges[1].end->val[1] - edges[1].begin->val[1], edges[1].end->val[2] - edges[1].begin->val[2]);
	normal = a.crossProduct(b);
}

Face::~Face() {
	delete edges;
}

Vector Face::getNormal() {
	return normal;
}

Vertex* Face::getVertices() {
	return vertices;
}

Edge* Face::getEdges() {
	return edges;
}

//Return null if the three planes don't intersect
Vertex* planePlanePlaneIntersection(Plane a, Plane b, Plane c) {
	Vector n1 = a.dir;
	Vector n2 = b.dir;
	Vector n3 = c.dir;
	return NULL;
}