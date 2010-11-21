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

	while(line.compare(numfaces) != 0)
		std::getline(fin,line);
	
	std::getline(fin,line);
	std::istringstream tkn(line);
	//gets the number of faces
	tkn >> numFaces;
	std::cout << numFaces << std::endl;
	faces = new Face[numFaces];
	//grab all the verticies
	std::getline(fin,line);
	std::getline(fin,line);
	std::istringstream int_num(line);
	int_num >> numVerts;
	std::cout << numVerts << std::endl;

    verticies = new Vertex[numVerts];
	int totalVerts = 0;
	for(int i = 0; i < numFaces; i++){
		std::getline(fin,line); //"face"
		std::getline(fin,line); //"numVerts"
		int numFaceVerts;
		std::getline(fin,line);
		std::istringstream faceV(line);
		faceV >> numFaceVerts;

		Vertex *faceVerts = new Vertex[numFaceVerts];

		for(int j = 0; j < numFaceVerts; j++){
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

			if(numFaces > 1){
				std::getline(tokenizer,token,',');
				std::istringstream float_z(token);
				float_z >> z;
			}
			else
				z = 0.0;

			faceVerts[j] = Vertex(x,y,z);
			verticies[totalVerts] = Vertex(x,y,z);
			totalVerts++;
		}
		faces[i] = Face(numFaceVerts, faceVerts);
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
		