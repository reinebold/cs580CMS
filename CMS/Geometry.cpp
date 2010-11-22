
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
    :connectedEdges(0),
     connectedFaces(0)
{
    val[X] = 0.0f;
    val[Y] = 0.0f;
    val[Z] = 0.0f;
    val[W] = 1.0f;

    edges[0] = NULL;
    edges[1] = NULL;
    edges[2] = NULL;
    edges[3] = NULL;

    faces[0] = NULL;
    faces[1] = NULL;
    faces[2] = NULL;
    faces[3] = NULL;
}

Vertex::Vertex(float xval, float yval, float zval, float wval)
    :connectedEdges(0),
     connectedFaces(0)
{
    val[X] = xval;
    val[Y] = yval;
    val[Z] = zval;
    val[W] = wval;

    edges[0] = NULL;
    edges[1] = NULL;
    edges[2] = NULL;
    edges[3] = NULL;

    faces[0] = NULL;
    faces[1] = NULL;
    faces[2] = NULL;
    faces[3] = NULL;
}

Vertex& Vertex::operator=(const Vertex &vert)
{
    val[X] = vert.val[X];
    val[Y] = vert.val[Y];
    val[Z] = vert.val[Z];
    val[W] = vert.val[W];

    connectedEdges = vert.connectedEdges;
    connectedFaces = vert.connectedFaces;

    edges[0] = vert.edges[0];
    edges[1] = vert.edges[1];
    edges[2] = vert.edges[2];
    edges[3] = vert.edges[3];

    faces[0] = vert.faces[0];
    faces[1] = vert.faces[1];
    faces[2] = vert.faces[2];
    faces[3] = vert.faces[3];
    
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
:faces(NULL),
vertices(NULL),
edges(NULL),
numVertices(0),
numFaces(0)
{

}

void Cuboid::init(int _numVertices, Vertex* _vertices)
{
	if(_numVertices != 8 && _numVertices != 4)
	{
		cerr << "Bounding box can only have 4 (for 2d case) or 8 (for 3d case) vertices. numVertices: " << numVertices << endl;
		exit(EXIT_FAILURE);
	}

    numVertices = _numVertices;

    vertices = _vertices;//new Vertex[numVertices];
    edges = new Edge[numVertices];

    //2D Case
    if(_numVertices == 4)
    {
        numFaces = 1;
        //Assign the edges (based on the vertices)
        for(int x = 0; x < numVertices; ++x)
        {
            int begin = x;
            int end;
            if(x+1 == numVertices)
            {
                end = 0;
            }
            else
            {
                end = x+1;
            }

            edges[x].begin = &vertices[begin];
            edges[x].end = &vertices[end];

            edges[x].updateEdgeState();
        }
    }
    else    //3D Case
    {
        numFaces = 6;
        faces = new Face[numFaces];

        faces[0].numVertices = 4;
        faces[0].numEdges = 4;
        faces[0].vertices = new Vertex*[4];
        faces[0].vertices[0] = &vertices[0];
        faces[0].vertices[1] = &vertices[1];
        faces[0].vertices[2] = &vertices[2];
        faces[0].vertices[3] = &vertices[3];
        faces[0].updateFaces();

        faces[1].numVertices = 4;
        faces[1].numEdges = 4;
        faces[1].vertices = new Vertex*[4];
        faces[1].vertices[0] = &vertices[0];
        faces[1].vertices[1] = &vertices[1];
        faces[1].vertices[2] = &vertices[5];
        faces[1].vertices[3] = &vertices[4];
        faces[1].updateFaces();

        faces[2].numVertices = 4;
        faces[2].numEdges = 4;
        faces[2].vertices = new Vertex*[4];
        faces[2].vertices[0] = &vertices[1];
        faces[2].vertices[1] = &vertices[2];
        faces[2].vertices[2] = &vertices[6];
        faces[2].vertices[3] = &vertices[5];
        faces[2].updateFaces();

        faces[3].numVertices = 4;
        faces[3].numEdges = 4;
        faces[3].vertices = new Vertex*[4];
        faces[3].vertices[0] = &vertices[2];
        faces[3].vertices[1] = &vertices[3];
        faces[3].vertices[2] = &vertices[7];
        faces[3].vertices[3] = &vertices[6];
        faces[3].updateFaces();

        faces[4].numVertices = 4;
        faces[4].numEdges = 4;
        faces[4].vertices = new Vertex*[4];
        faces[4].vertices[0] = &vertices[3];
        faces[4].vertices[1] = &vertices[0];
        faces[4].vertices[2] = &vertices[4];
        faces[4].vertices[3] = &vertices[7];
        faces[4].updateFaces();

        faces[5].numVertices = 4;
        faces[5].numEdges = 4;
        faces[5].vertices = new Vertex*[4];
        faces[5].vertices[0] = &vertices[4];
        faces[5].vertices[1] = &vertices[5];
        faces[5].vertices[2] = &vertices[6];
        faces[5].vertices[3] = &vertices[7];
        faces[5].updateFaces();
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


Vector::Vector(float inX, float inY, float inZ) 
{
	x = inX;
	y = inY;
	z = inZ;
}

Vector::Vector() {
	x = 0;
	y = 0;
	z = 0;
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

Face::Face()
:numVertices(0),
 numEdges(0)
{

}

Face::Face(int _numVertices, Vertex** _vertices)
{
	numVertices = _numVertices;
	numEdges = _numVertices;
	vertices = _vertices;



}

Face::~Face() {
//	delete edges;
}

void Face::updateFaces()
{
    edges = new Edge[numEdges];
    for(int i=0; i < numVertices; i++) 
    {
        Vertex* v1 = vertices[i];
        Vertex* v2;
        if (i == numVertices - 1) 
        {
            v2 = vertices[0];
        } 
        else 
        {
            v2 = vertices[i + 1];
        }
        
        edges[i] = Edge(v1, v2);;
    }
    Vector a(edges[0].end->val[0] - edges[0].begin->val[0], edges[0].end->val[1] - edges[0].begin->val[1], edges[0].end->val[2] - edges[0].begin->val[2]);
    Vector b(edges[1].end->val[0] - edges[1].begin->val[0], edges[1].end->val[1] - edges[1].begin->val[1], edges[1].end->val[2] - edges[1].begin->val[2]);
    normal = a.crossProduct(b);
}

Face& Face::operator=(const Face &_face)
{
    numVertices = _face.numVertices;
    numEdges = _face.numEdges;
    normal = _face.normal;
    
    //Vertex** vertices;  //array of Vertex*
    //Edge* edges;
    return *this;
}

//Return null if the three planes don't intersect
Vertex* planePlanePlaneIntersection(Plane a, Plane b, Plane c) {
	Vector n1 = a.dir;
	Vector n2 = b.dir;
	Vector n3 = c.dir;
	return NULL;
}

bool Vertex::operator==(const Vertex &vert)
{
    if(vert.val[X] == val[X] && vert.val[Y] == val[Y] && vert.val[Z] == val[Z])
    {
        return true;
    }
    else
    {
        return false;
    }
}


/*Returns a new (dynamically allocated) Edge (with the two endpoints).*/
Edge* Geometry::planeFaceIntersection(const Plane &plane, const Face &face) {
	return NULL;
}

//Takes in four edges. Returns new (dynamically allocated Face)
Face* createFace(Edge *edge, const int numEdges) {
	Vertex** vertexArray = new Vertex*[numEdges];
	for(int i=0; i < numEdges; i++) {
		vertexArray[i] = edge[i].begin;
	}
	Face* f = new Face(numEdges, vertexArray);
	f->updateFaces();
	return f;
}

//Returns vertex of the intersection.  Make sure you fill the 'faces' data member that points to the faces that intersected it so we can find it's normals.
Vertex* faceFaceFaceIntersection(const Face &face1, const Face &face2, const Face &face3) {
	return NULL;
}

//Tells you if vertex is in sphere.  Simple I think....return (vert.val[X]-center.val[X])^2+(vert.val[Y] - center.val[Y])^2+(vert.val[Z] - center.val[Z])^2 <= radius^2
bool vertexInSphere(Vertex *center, Vertex *vert, float radius) {
	return pow(vert->val[X] - center->val[X], 2)+pow(vert->val[Y] - center->val[Y], 2)+ pow(vert->val[Z] - center->val[Z], 2) <= pow(radius, 2);
}