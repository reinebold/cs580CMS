#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "Parser.h"
#include "Geometry.h"
using namespace Geometry;

Parser::Parser(){

}

void Parser::vertexArray(int &numVerts, Vertex* &verticies, int &numFaces, Face* &faces){
	std::ifstream fin("../CMS/input.model");
	std::string line;
	std::string numfaces("numfaces");

	for(;;){
		std::getline(fin,line);
		if(line.compare(numfaces) == 0)
			break;
	}
	
	std::getline(fin,line);
	std::istringstream tkn(line);
	//gets the number of faces for future usage, but not used anywhere yet.
	tkn >> numFaces;
	std::cout << numFaces << std::endl;

	//grab all the verticies
	std::getline(fin,line);
	std::getline(fin,line);
	std::istringstream int_num(line);
	int_num >> numVerts;
	std::cout << numVerts << std::endl;

    verticies = new Vertex[numVerts];
	
	for(int i = 0; i < numVerts; i++){
		float x,y;
		std::getline(fin,line);
		std::istringstream tokenizer(line);
		std::string token;

		std::getline(tokenizer,token,',');
		std::istringstream float_x(token);
		float_x >> x;
		std::cout << x << std::endl;

		std::getline(tokenizer,token,',');
		std::istringstream float_y(token);
		float_y >> y;
		std::cout << y << std::endl;

		verticies[i] = Vertex(x,y);
	}
	fin.close();

}


void Parser::boundingBox(int &numBoundVerts, Vertex* &bounds){
	std::ifstream fin("../CMS/input.model");
	std::string line;

	std::getline(fin,line); //bound
	std::getline(fin,line); //bound size
	std::istringstream bnds(line);
	int temp;
	bnds >> temp;
	numBoundVerts = temp;

	bounds = new Vertex[numBoundVerts];
	//obtain bounding box
	for(int i = 0; i < numBoundVerts; i++){
		std::getline(fin,line);
	    std::istringstream tokenizer(line);
		std::string token;

		std::getline(tokenizer,token,',');
		std::istringstream float_x(token);
		float_x >> bounds[i].val[X];

		std::getline(tokenizer,token,',');
		std::istringstream float_y(token);
		float_y >> bounds[i].val[Y];

		std::getline(tokenizer,token,',');
		std::istringstream float_z(token);
		float_z >> bounds[i].val[Z];
	}
	fin.close();
}
		