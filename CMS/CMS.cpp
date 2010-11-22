/**
    \file   CMS
    \brief  
*/

#include "CMS.h"
#include "CMS2d.h"
#include "Lights.h"
#include "State.h"
#include "Program.h"
#include "CMSModel.h"
#include "Geometry.h"
#include "Grid.h"
#include "Camera.h"
#include "Parser.h"
#include "Utilities.h"
#include <time.h>
using namespace Geometry;

extern Lights lights;
extern State state;
extern Camera camera;

CMS::CMS()
:showBoundingBox(true),
 showGridVertices(true)
{

}

void CMS::continuousModelSynthesis(vector<Edge*> &edges, vector<Vertex*> &vertices)
{
    while(!CMS2D::continuousModelSynthesis2D(edges, vertices, input, grid))
      std::cout << "Algorithm did not converge. Restarting" << std::endl << std::endl;
}

void CMS::display3D()
{
    //Display the bounding box
    glTranslatef(12.8f, 11.6f, 0.0f);
    glScalef(.46f, .46f, .46f);
    if(showBoundingBox)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            for(int x = 0; x < boundingBox.numFaces; x++)
            {
                glNormal3f(boundingBox.faces[x].normal.x,boundingBox.faces[x].normal.y,boundingBox.faces[x].normal.z);
                for(int y = 0; y < boundingBox.faces[x].numVertices; y++)
                {
                    glVertex3fv((*boundingBox.faces[x].vertices[y]).val);
                }
            }
        glEnd();
    }

    //Display the input model.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTranslatef(-10.0f, 0.0f, 0.0f);

    //The input model's edges.
    for(int numFaces = 0; numFaces < input3D.numFaces; ++numFaces)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glLineWidth(1.4f);
        glBegin(GL_LINES);
        for(int numEdges = 0; numEdges < input3D.faces[numFaces].numEdges; ++numEdges)
        {
            glVertex3fv(input3D.faces[numFaces].edges[numEdges].begin->val);
            glVertex3fv(input3D.faces[numFaces].edges[numEdges].end->val);
        }
        glEnd();
    }

    //The input model's points.
    for(int numVerts = 0; numVerts < input3D.numVertices; ++numVerts)
    {
        glEnable(GL_POINT_SMOOTH);  //Make the point a sphere basically.
        glPointSize(4.0f);          //Change the size of the point
        glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
        glBegin(GL_POINTS);
        glVertex3fv(input3D.vertices[numVerts].val);
        glEnd();
    }

    //The input model's faces.  Must be in correct order for correct transparency effect.
    for(int numFaces = 0; numFaces < input3D.numFaces; ++numFaces)
    {
        glBegin(GL_POLYGON);
            glColor4fv(input3D.colors[numFaces].val);    
            for(int numVertices = 0; numVertices < input3D.faces[numFaces].numVertices; ++numVertices)
            {
                glVertex3fv((*input3D.faces[numFaces].vertices[numVertices]).val);  
            }
        glEnd();
    }
    glTranslatef(10.0f,0.0f,0.0f);
}

void CMS::display2D()
{   
    //Draw Bounding Box
    if(showBoundingBox)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3fv(boundingBox.vertices[0].val);
            glVertex3fv(boundingBox.vertices[1].val);
            glVertex3fv(boundingBox.vertices[2].val);
            glVertex3fv(boundingBox.vertices[3].val);
        glEnd();
    }
   
    //Draw input model (translate it over so it's not in the bounding box...)
    glTranslatef(-10.0f, 0.0f, 0.0f);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex3fv(input.vertices[0].val);  
        glVertex3fv(input.vertices[1].val);
        
        glColor4f(0.25f, 0.25f, 1.0f, 1.0f);
        glVertex3fv(input.vertices[1].val);
        glVertex3fv(input.vertices[2].val);
        
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glVertex3fv(input.vertices[2].val);
        glVertex3fv(input.vertices[0].val);
    glEnd();
    glTranslatef(10.0f,0.0f,0.0f);
   
    for(int x = 0; x < (int)grid.edges.size(); x++)
    {
        //if(grid.edges[x]->edgestate.set == 0)
        //{
        //     glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        //}
        //else if(grid.edges[x]->edgestate.set == 1)
        //{
        //    glColor4f(0.25f, 0.25f, 1.0f, 1.0f);
        //}
        //else
        //{
        //    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        //}
        //Edge
        if(grid.edges[x]->edgestate.leftFace != grid.edges[x]->edgestate.rightFace)
        {
            glColor4f(0.5f, 0.9f, 0.5f, 1.0f);
        }
        //Either interior or exterior
        else
        {
          //Interior
          if(grid.edges[x]->edgestate.leftFace == INTERIOR)
            glColor4f(0.9f, 0.9f, 0.9f, 1.0f);
          //Exterior
          else
            glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
        }

        //Error Check
        if(grid.edges[x]->edgestate.leftFace == UNASSIGNED ||
          grid.edges[x]->edgestate.rightFace == UNASSIGNED)
          glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

        glBegin(GL_LINES);
            glVertex3fv(grid.edges[x]->begin->val);
            glVertex3fv(grid.edges[x]->end->val);
        glEnd();
    }

    //Draw Vertices
    if(showGridVertices)
    {
        glEnable(GL_POINT_SMOOTH);  //Make the point a sphere basically.
        glPointSize(4.0f);          //Change the size of the point
        glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
        glBegin(GL_POINTS);
            for(int x=0; x < (int)grid.vertices.size(); x++)
            {
                glVertex3fv(grid.vertices[x]->val);
            }
        glEnd();
        glDisable(GL_POINT_SMOOTH);
    }
}

void CMS::init()
{
    state.setWireFrame(true);
    state.setLighting(false);
    state.setDrawLights(false);
    state.setDrawAxis(false);
    state.setPrintInfoOnScreen(true);
    
    //unsigned int seed = (unsigned int)time(NULL);
    unsigned int seed = 1290455743;
    srand(seed);
    cout << "Seed value: " << seed << endl;

	//Bounding box info
	int bbnumVertices;
	Vertex* bbvertices = NULL;  //Should be deleted in boundingbox.init
	parser.boundingBox(bbnumVertices, bbvertices);
    boundingBox.init(bbnumVertices, bbvertices);

	//Vertex info: Must be specified in a counter-clockwise order
	int numVertices;
	int numFaces;
	Face* faces = NULL;
	Vertex *vertices = NULL;
	parser.vertexArray(numVertices, vertices, numFaces, faces);

    if(numFaces == 1)
    {   
        //Load the vertices into the input model, find edges.
        input.init(numFaces, numVertices, vertices);
        delete [] faces->vertices;
        delete [] faces;
        delete [] vertices;

        //Figure out edges and vertices.
        grid.init(input, boundingBox);

        continuousModelSynthesis(grid.edges, grid.vertices);
    }
    else
    {
        input3D.init(numVertices, vertices, numFaces, faces);

        grid.init(input3D, boundingBox);
    }

    //Do the algorithm that changes the states.
    //continuousModelSynthesis(grid.edges, grid.vertices);
}