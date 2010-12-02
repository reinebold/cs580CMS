#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "Parser.h"
#include "Geometry.h"
using namespace Geometry;

Parser::Parser(){

}

void Parser::vertexArray(string filename, int &numVerts, Vertex* &vertices, int &numFaces, Face* &faces){
	std::ifstream fin(filename.c_str());
	std::string line;
	std::string numfaces("numfaces");

	while(line.compare(numfaces) != 0)
		std::getline(fin,line);
	
	std::getline(fin,line);
	std::istringstream tkn(line);
	//gets the number of faces
	tkn >> numFaces;
	faces = new Face[numFaces];

	//grab all the verticies
	std::getline(fin,line);
	std::getline(fin,line);
	std::istringstream int_num(line);
	int_num >> numVerts;

    vertices = new Vertex[numVerts];

	int totalVerts = 0;
	for(int i = 0; i < numFaces; i++){
		std::getline(fin,line); //"face"
		std::getline(fin,line); //"numVerts"
		int numFaceVerts;
		std::getline(fin,line);
		std::istringstream faceV(line);
		faceV >> numFaceVerts;

		Vertex *faceVerts = new Vertex[numFaceVerts];
        
        faces[i].vertices = new Vertex*[numFaceVerts];
		for(int j = 0; j < numFaceVerts; j++)
        {
            faces[i].numVertices = numFaceVerts;
            faces[i].numEdges = numFaceVerts;
            
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
            {
				z = 0.0;
            }

            int k;
            Vertex temp(x,y,z,0.5f);
            for(k = 0; k < totalVerts; k++)     //Find out if the vertex was already assigned.
            {
                if(vertices[k] == temp)
                {
                    break;
                }
            }

            if(k == totalVerts) //If not, assign it.
            {
                vertices[k] = temp;
                totalVerts++;
            }
            vertices[k].faces[vertices[k].connectedFaces++] = &faces[i];
            faces[i].vertices[j] = &vertices[k];
		}
		faces[i].updateFaces();
	}
	fin.close();

}


void Parser::boundingBox(string filename, int &numBoundVerts, Vertex* &bounds){
	std::ifstream fin(filename.c_str());
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
		