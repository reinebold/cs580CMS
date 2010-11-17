#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "Parser.h"
#include "Geometry.h"
using namespace Geometry;

Parser::Parser(){

}

Vertex* Parser::vertexArray(int *numVerts,int *numFaces){
	std::ifstream fin("../CMS/input.model");
	std::string line;

	for(int i = 0; i < 5; i++)
		std::getline(fin,line);

	std::getline(fin,line);
	std::getline(fin,line);
	std::istringstream tkn(line);
	//gets the number of faces for future usage, but not used anywhere yet.
	tkn >> *numFaces;

	//grab all the verticies
	std::getline(fin,line);
	std::getline(fin,line);
	std::istringstream int_num(line);
	int_num >> *numVerts;
	std::cout << *numVerts << std::endl;
	Vertex* verticies = new Vertex[*numVerts];
	for(int i = 0; i < *numVerts; i++){
		float x,y;
		std::getline(fin,line);
		std::istringstream tokenizer(line);
		std::string token;

		std::getline(tokenizer,token,',');
		std::istringstream float_x(token);
		float_x >> x;

		std::getline(tokenizer,token,',');
		std::istringstream float_y(token);
		float_y >> y;

		verticies[i] = Vertex(x,y);
	}
	fin.close();
	return verticies;
}

void Parser::boundingBox(Vertex* bounds){
	std::ifstream fin("../CMS/input.model");
	std::string line;

	std::getline(fin,line);
	
	//obtain bounding box
	for(int i = 0; i < 4; i++){
		float x,y,z;
		std::getline(fin,line);
		std::istringstream tokenizer(line);
		std::string token;

		std::getline(tokenizer,token,',');
		std::istringstream float_x(token);
		float_x >> x;

		std::getline(tokenizer,token,',');
		std::istringstream float_y(token);
		float_y >> y;

		std::getline(tokenizer,token,',');
		std::istringstream float_z(token);
		float_z >> z;

		bounds[i] = Vertex(x,y,z);
	}
	fin.close();
}
		