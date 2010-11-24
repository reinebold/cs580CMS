
#include "Defines.h"
#include "Geometry.h"
#include <math.h>
#include <vector>
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
numFaces(0),
width(0.0f),
height(0.0f),
depth(0.0f)
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

    for(int x = 0; x < numFaces; x++)
    {
        for(int y = 0; y < faces[x].numEdges; y++)
        {
            float calcWidth  = fabs(faces[x].edges[y].begin->val[X]-faces[x].edges[y].end->val[X]);
            if(calcWidth > width)
            {
                width = calcWidth;
            }

            float calcHeight = fabs(faces[x].edges[y].begin->val[Y]-faces[x].edges[y].end->val[Y]);
            if(calcHeight > height)
            {
                height = calcHeight;
            }

            float calcDepth  = fabs(faces[x].edges[y].begin->val[Z]-faces[x].edges[y].end->val[Z]);
            if(calcDepth > depth)
            {
                depth = calcDepth;
            }
        }
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

float Vector::dotProduct(const Vector& other) const {
	return x * other.x + y * other.y + z * other.z;
}

Vector& Vector::operator=(const Vector& other) {
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

Vector Vector::crossProduct(const Vector& other) const {
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
 numEdges(0),
 set(-1)
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

float Face::calcAngle(const Vector &v1, const Vector &v2, const Vector &normal)
{
    Vector c;
    c = v1.crossProduct(v2);
    float angle = atan2(c.magnitude(), v1.dotProduct(v2));
    return (c.dotProduct(normal) < 0.0f) ? -angle : angle;
}

void Face::sortVertices()
{
    //MUST HAVE NORMAL CALCULATED!
    Vector average(0,0,0);
    for(int i = 0; i < numVertices; i++)
    {
	    average.x += vertices[i]->val[X];
        average.y += vertices[i]->val[Y];
        average.z += vertices[i]->val[Z];
    }
    average.x /= numVertices;
    average.y /= numVertices;
    average.z /= numVertices;

    float *angles = new float[numVertices];
    angles[0] = 0.0f;
    Vector vec1;
    vec1.x = vertices[0]->val[X];
    vec1.y = vertices[0]->val[Y];
    vec1.z = vertices[0]->val[Z];

    for (int i = 1; i < numVertices; ++i) 
    {
        Vector vec2;
        vec2.x = vertices[i]->val[X];
        vec2.y = vertices[i]->val[Y];
        vec2.z = vertices[i]->val[Z];
        angles[i] = calcAngle(vec1 - average, vec2 - average, normal);
    }

    //Vertex **verts = new Vertex*[numVertices];
    float smallest = 100000;                //TODO: FIND BETTER;
    int order[4] = {0};
    int current = 0;
    int smallestindex = 0;
    for(int y = 0; y < numVertices; y++)
    {
        for(int x = 0; x < numVertices; x++)
        {
            if(angles[x] < smallest)
            {
                smallest = angles[x];
                smallestindex = x;
            }
           
        }
        order[current++] = smallestindex;
        angles[smallestindex] = 1000000;
        smallest = 100000;
    }

    for(int x = 0; x < numVertices; x++)
    {
        Vertex *temp = vertices[x];
        vertices[x] = vertices[order[x]];
        vertices[order[x]] = temp;
    }
}

Vector Vector::operator-(const Vector &rhs)
{
    Vector result;
    result.x = x - rhs.x;
    result.y = y - rhs.y;
    result.z = z - rhs.z;
    return result;
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
        
        edges[i] = Edge(v1, v2);
    }

    Vector a(edges[0].end->val[0] - edges[0].begin->val[0], edges[0].end->val[1] - edges[0].begin->val[1], edges[0].end->val[2] - edges[0].begin->val[2]);
    Vector b(edges[1].end->val[0] - edges[1].begin->val[0], edges[1].end->val[1] - edges[1].begin->val[1], edges[1].end->val[2] - edges[1].begin->val[2]);
    normal = a.crossProduct(b);
    normal.normalize();
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

bool Vector::operator==(const Vector& rhs)
{
    return ((fabs(x - rhs.x) < EPSILON) && (fabs(y - rhs.y) < EPSILON) && (fabs(z - rhs.z) < EPSILON));
}

Vector Vector::operator*(float f) {
    Vector answer(this->x * f, this->y * f, this->z * f);
	return answer;
}

Vector& Vector::operator*=(float f) {
    this->x *= f;
	this->y *= f;
	this->z *= z;
	return *this;
}

Vector& Vector::operator+=(const Vector& other) {
	this->x += other.x;
	this->y += other.y;
	this->z += other.z;
	return *this;
}

Vector& Vector::operator/=(float f) {
	this->x /= f;
	this->y /= f;
	this->z /= z;
	return *this;
}


Vector* Vector::normalize()
{
    float magnitude = sqrt(x*x + y*y + z*z);
    x = x/magnitude;
    y = y/magnitude;
    z = z/magnitude;

    return this;
}

/*Returns a new (dynamically allocated) Edge (with the two endpoints).*/
Edge* Geometry::planeFaceIntersection(const Plane &plane, const Face &face) {

	//given a plane and a face
	//face has 4 edges
	//take each of those edges, do plane line intersection
	//2 of edges will give intersection
	//2 will give a null
	//take the 2 points and combine them for an edge
	Edge edge1 = face.edges[0];
	Edge edge2 = face.edges[1];
	Edge edge3 = face.edges[2];
	Edge edge4 = face.edges[3];

	std::vector<Vertex*> vertices;

	for(int i=0; i < 4; i++) {
		Edge e = face.edges[i];
		Vector denomVector(e.end->val[X] - e.begin->val[X], e.end->val[Y] - e.begin->val[Y], e.end->val[Z] - e.begin->val[Z]);
		Vector numVec(plane.v.val[X] - e.begin->val[X], plane.v.val[Y] - e.begin->val[Y], plane.v.val[Z] - e.begin->val[Z]);
		float uDenom = denomVector.dotProduct(plane.dir);
		float uNum = numVec.dotProduct(plane.dir);
		if (uDenom != 0) {
			float u = uNum / uDenom;
			if (u >= 0 && u <= 1) {
				Vertex* v1 = new Vertex(e.begin->val[X] + (u * (e.end->val[X] - e.begin->val[X])), e.begin->val[Y] + (u * (e.end->val[Y] - e.begin->val[Y])), e.begin->val[Z] + (u * (e.end->val[Z] - e.begin->val[Z])));
				vertices.push_back(v1);
			}
		}

	}

	if (vertices.size() != 2) {
		return NULL;
	} else {
		Edge* e = new Edge(vertices.at(0), vertices.at(1));
		return e;
	}

	//http://en.wikipedia.org/wiki/Line-plane_intersection
	//need direction vector of the line segment
	//http://local.wasp.uwa.edu.au/~pbourke/geometry/planeline/
}

//Takes in four edges. Returns new (dynamically allocated Face)
Face* Geometry::createFace(Edge *edges[], const int numEdges) {
	Vertex** vertexArray = new Vertex*[numEdges];
	for(int i=0; i < numEdges; i++) 
    {
		vertexArray[i] = edges[i]->begin;
	}
#ifdef _DEBUG
    //Sanity Check: Make sure vertices are different;
    for(int x = 0; x < numEdges; x++)
    {
        for(int y = 0; y < numEdges; y++)
        {
            if(x == y)
            {
                continue;
            }
            if(vertexArray[x] == vertexArray[y])
            {
                cout << "Error: Vertices are the same when trying to create a face." << endl;
            }
        }
    }
#endif
	Face* f = new Face(numEdges, vertexArray);
	f->updateFaces();
	return f;
}

//Returns vertex of the intersection.  Make sure you fill the 'faces' data member that points to the faces that intersected it so we can find it's normals.
//http://www.cgafaq.info/wiki/Intersection_of_three_planes
//Returns NULL on a failed intersection
Vertex* Geometry::faceFaceFaceIntersection(Face *face1, Face *face2, Face *face3) {
	if (face1 == NULL || face2 == NULL || face3 == NULL) {
		return NULL;
	}
	//need a point from each of the faces, so just use the first vertex
	Vector p1(face1->vertices[0]->val[X], face1->vertices[0]->val[Y], face1->vertices[0]->val[Z]);
	Vector p2(face2->vertices[0]->val[X], face2->vertices[0]->val[Y], face2->vertices[0]->val[Z]);
	Vector p3(face3->vertices[0]->val[X], face2->vertices[0]->val[Y], face3->vertices[0]->val[Z]);
	float d1 = face1->normal.dotProduct(p1);
	float d2 = face2->normal.dotProduct(p2);
	float d3 = face3->normal.dotProduct(p3);
	Vector v1 = face2->normal.crossProduct(face3->normal);
	v1 *= d1;
	Vector v2 = face3->normal.crossProduct(face1->normal);
	v2 *= d2;
	Vector v3 = face1->normal.crossProduct(face2->normal);
	v3 *= d3;
	v1 += v2;
	v1 += v3;
	//v1 is now the numerator
	Vector denominatorVec = face2->normal.crossProduct(face3->normal);
	float denominator = denominatorVec.dotProduct(face1->normal);
	if (denominator == 0) {
		//failed intersection test
		return NULL;
	} else {
		v1 /= denominator;
		Vertex* answer = new Vertex(v1.x, v2.y, v2.z);
		answer->faces[0] = face1;
		answer->faces[1] = face2;
		answer->faces[2] = face3;
		return answer;
	}
}

//Tells you if vertex is in sphere.  Simple I think....return (vert.val[X]-center.val[X])^2+(vert.val[Y] - center.val[Y])^2+(vert.val[Z] - center.val[Z])^2 <= radius^2
bool Geometry::vertexInSphere(Vertex *center, Vertex *vert, float radius) {
	return pow(vert->val[X] - center->val[X], 2)+pow(vert->val[Y] - center->val[Y], 2)+ pow(vert->val[Z] - center->val[Z], 2) <= pow(radius, 2);
}

float Vector::magnitude()
{
    return sqrt(x*x+y*y+z*z);
}
