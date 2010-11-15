/**
    \file   CMS
    \brief  
*/

#include "CMS.h"
#include "Lights.h"
#include "State.h"
#include "Program.h"
#include "CMSModel.h"
#include "Geometry.h"
#include "Grid.h"
#include "Camera.h"
#include <time.h>
using namespace Geometry;

extern Lights lights;
extern State state;
extern Camera camera;
CMS::CMS()
{

}

void CMS::continuousModelSynthesis(vector<Edge*> &edges, vector<Vertex*> &verticies)
{


}

void CMS::display()
{   
    //Draw Bounding Box
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3fv(boundingbox.verticies[0].val);
        glVertex3fv(boundingbox.verticies[1].val);
        glVertex3fv(boundingbox.verticies[2].val);
        glVertex3fv(boundingbox.verticies[3].val);
    glEnd();
   
    //Draw input model (translate it over so it's not in the bounding box...)
    glTranslatef(-10.0f, 0.0f, 0.0f);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex3fv(input._verticies[0].val);  
        glVertex3fv(input._verticies[1].val);
        
        glColor4f(0.25f, 0.25f, 1.0f, 1.0f);
        glVertex3fv(input._verticies[1].val);
        glVertex3fv(input._verticies[2].val);
        
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glVertex3fv(input._verticies[2].val);
        glVertex3fv(input._verticies[0].val);
    glEnd();
    glTranslatef(10.0f,0.0f,0.0f);
   
    for(int x = 0; x < (int)grid.edges.size(); x++)
    {
        if(grid.edges[x]->edgestate.set == 0)
        {
             glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else if(grid.edges[x]->edgestate.set == 1)
        {
            glColor4f(0.25f, 0.25f, 1.0f, 1.0f);
        }
        else
        {
            glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        }
        glBegin(GL_LINES);
            glVertex3fv(grid.edges[x]->begin->val);
            glVertex3fv(grid.edges[x]->end->val);
        glEnd();
        
    }

    //Draw Verticies
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

void CMS::init()
{
    state.setWireFrame(true);
    state.setLighting(false);
    state.setDrawLights(false);
    state.setDrawAxis(false);
    state.setPrintInfoOnScreen(true);
    
    srand((unsigned int)time(NULL));

    //Bounding box info (will get from text file)
    Vertex bbverticies[4];
    bbverticies[0] = Vertex(0.0f,  0.0f,  0.0f);
    bbverticies[1] = Vertex(50.0f, 0.0f,  0.0f);
    bbverticies[2] = Vertex(50.0f, 50.0f, 0.0f);
    bbverticies[3] = Vertex(0.0f,  50.0f, 0.0f);

    //Input file info (will get from text file)
    int numVerticies = 3;
    Vertex *verticies = new Vertex[numVerticies];

    //Must be specified in a counter-clockwise order?
    verticies[0] = Vertex(0.0f, 0.0f);
    verticies[1] = Vertex(5.0f, 0.0f);
    verticies[2] = Vertex(3.0f, 2.0f);

    //Figure out the edges of the bounding box
    boundingbox.init(bbverticies);

    //Load the verticies into the input model, find edges.
    input.init(numVerticies, verticies);

    //Figure out edges and verticies.
    grid.init(input, boundingbox);

    //Do the algorithm that changes the states.
    continuousModelSynthesis(grid.edges, grid.verticies);
}