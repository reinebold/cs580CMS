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
#include <time.h>
using namespace Geometry;

extern Lights lights;
extern State state;
extern Camera camera;

CMS::CMS()
:showBoundingBox(true),
 showGridVerticies(true)
{

}

void CMS::continuousModelSynthesis(vector<Edge*> &edges, vector<Vertex*> &verticies)
{
    while(!CMS2D::continuousModelSynthesis2D(edges, verticies, input, grid))
      std::cout << "Algorithm did not converge. Restarting" << std::endl << std::endl;
}

void CMS::display3D()
{
    if(showBoundingBox)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3fv(boundingBox.verticies[0].val);
            glVertex3fv(boundingBox.verticies[1].val);
            glVertex3fv(boundingBox.verticies[2].val);
            glVertex3fv(boundingBox.verticies[3].val);

            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3fv(boundingBox.verticies[0].val); 
            glVertex3fv(boundingBox.verticies[1].val);
            glVertex3fv(boundingBox.verticies[5].val);
            glVertex3fv(boundingBox.verticies[4].val);

            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3fv(boundingBox.verticies[1].val);
            glVertex3fv(boundingBox.verticies[2].val);
            glVertex3fv(boundingBox.verticies[6].val);
            glVertex3fv(boundingBox.verticies[5].val);

            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3fv(boundingBox.verticies[2].val);
            glVertex3fv(boundingBox.verticies[3].val);
            glVertex3fv(boundingBox.verticies[7].val);
            glVertex3fv(boundingBox.verticies[6].val);

            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3fv(boundingBox.verticies[3].val);
            glVertex3fv(boundingBox.verticies[0].val);
            glVertex3fv(boundingBox.verticies[4].val);
            glVertex3fv(boundingBox.verticies[7].val);

            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3fv(boundingBox.verticies[4].val);
            glVertex3fv(boundingBox.verticies[5].val);
            glVertex3fv(boundingBox.verticies[6].val);
            glVertex3fv(boundingBox.verticies[7].val);
        glEnd();
    }

    //Output the model
    glTranslatef(-10.0f, 0.0f, 0.0f);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);    
    for(int numFaces = 0; numFaces < input.numFaces; ++numFaces)
    {
        glBegin(GL_POLYGON);
        for(int numVertices = 0; numVertices < input.numVerticies; ++numVerticies)
        {
            glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
            glVertex3fv(input.verticies[numFaces][numVerticies].val);  
            glVertex3fv(input.verticies[numFaces][numVerticies].val);
            
            glColor4f(0.25f, 0.25f, 1.0f, 0.5f);
            glVertex3fv(input.verticies[numFaces][numVerticies].val);
            glVertex3fv(input.verticies[2].val);
            
            glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
            glVertex3fv(input.verticies[2].val);
            glVertex3fv(input.verticies[0].val);
        }
        glEnd();
    }
    
    glTranslatef(10.0f,0.0f,0.0f);
   
    /*for(int x = 0; x < (int)grid.edges.size(); x++)
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

    //Draw Verticies
    if(showGridVerticies)
    {
        glEnable(GL_POINT_SMOOTH);  //Make the point a sphere basically.
        glPointSize(4.0f);          //Change the size of the point
        glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
        glBegin(GL_POINTS);
            for(int x=0; x < (int)grid.verticies.size(); x++)
            {
                glVertex3fv(grid.verticies[x]->val);
            }
        glEnd();
        glDisable(GL_POINT_SMOOTH);
    }*/      
}

void CMS::display2D()
{   
    //Draw Bounding Box
    if(showBoundingBox)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3fv(boundingBox.verticies[0].val);
            glVertex3fv(boundingBox.verticies[1].val);
            glVertex3fv(boundingBox.verticies[2].val);
            glVertex3fv(boundingBox.verticies[3].val);
        glEnd();
    }
   
    //Draw input model (translate it over so it's not in the bounding box...)
    glTranslatef(-10.0f, 0.0f, 0.0f);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex3fv(input.verticies[0].val);  
        glVertex3fv(input.verticies[1].val);
        
        glColor4f(0.25f, 0.25f, 1.0f, 1.0f);
        glVertex3fv(input.verticies[1].val);
        glVertex3fv(input.verticies[2].val);
        
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glVertex3fv(input.verticies[2].val);
        glVertex3fv(input.verticies[0].val);
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

    //Draw Verticies
    if(showGridVerticies)
    {
        glEnable(GL_POINT_SMOOTH);  //Make the point a sphere basically.
        glPointSize(4.0f);          //Change the size of the point
        glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
        glBegin(GL_POINTS);
            for(int x=0; x < (int)grid.verticies.size(); x++)
            {
                glVertex3fv(grid.verticies[x]->val);
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
    
    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);
    cout << "Seed value: " << seed << endl;

	//Bounding box info
	int bbnumVerticies;
	Vertex* bbverticies;
	parser.boundingBox(bbnumVerticies, bbverticies);

	//Figure out the edges of the bounding box
    boundingBox.init(bbnumVerticies, bbverticies);

	//vertex info: Must be specified in a counter-clockwise order
	int numVerticies;
	int numFaces;
	Face* faces;
	Vertex *verticies;
	parser.vertexArray(numVerticies, verticies, numFaces, faces);

    //Load the verticies into the input model, find edges.
    input.init(numFaces, numVerticies, verticies);

    //Figure out edges and verticies.
    grid.init(input, boundingBox);

    //Do the algorithm that changes the states.
    continuousModelSynthesis(grid.edges, grid.verticies);
}