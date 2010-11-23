#ifndef _PARSER_H
#define _PARSER_H

#include <fstream>
#include <sstream>
#include <iostream>
#include "Geometry.h"
using namespace Geometry;

class Parser
{
public:
	Parser();

	void boundingBox(string filename, int &numBoundVerts, Vertex* &bounds);
	void vertexArray(string filename, int &numVerts, Vertex* &vertices, int &numFaces, Face* &faces);

};
#endif
