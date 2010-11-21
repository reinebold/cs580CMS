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

	void boundingBox(int &numBoundVerts, Vertex* bounds);
	Vertex* vertexArray(int &numVerts, int &numFaces);

};
#endif
