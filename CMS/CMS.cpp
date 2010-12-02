/**
    \file   CMS
    \brief  
*/

#include "CMS.h"
#include "CMS2d.h"
#include "CMS3d.h"
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
#include <IL/il.h>
#include <IL/ilu.h>
using namespace Geometry;

extern Lights lights;
extern State state;
extern Camera camera;

CMS::CMS()
:showBoundingBox(true),
 showGridVertices(false),
 showGridEdges(false),
 showGridFaces(false),
 showGridVolumes(false),
 showTexture(false),
 recreateCity(false),
 showModel(true)
{

}

void CMS::continuousModelSynthesis(vector<Edge*> &edges, vector<Vertex*> &vertices)
{
    while(!CMS2D::continuousModelSynthesis2D(edges, vertices, input, grid))
      std::cout << "Algorithm did not converge. Restarting" << std::endl << std::endl;
}

void CMS::continuousModelSynthesis3D(vector<Edge*> &edges, vector<Vertex*> &vertices)
{
  CMS3D::continuousModelSynthesis(edges, vertices, input3D, grid);
}

void CMS::display3D()
{
    //Show bounding box
    const float epsilon =  .01f;
    if(showBoundingBox)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

    float sizex = boundingBox.width*4;
    float sizey = boundingBox.height;
    float sizez = boundingBox.width*4;
    
    //Show world
    if(showTexture)
    {
        glEnable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glTranslatef(-(sizex - boundingBox.width)/2.0f, 0.0f, (sizez - boundingBox.depth)/2.0f);

        glBindTexture(GL_TEXTURE_2D,texSkyBox[2].getId());
        glBegin(GL_QUADS);
            glNormal3f(0.0f,1.0f,0.0f);
            glTexCoord2f (0.0, 0.0);
            glVertex3f(0.0f,0.0f,0.0f);
            glTexCoord2f (1.0, 0.0);
            glVertex3f(sizex,0.0f,0.0f);
            glTexCoord2f (1.0, 1.0);
            glVertex3f(sizex,0.0f,-sizez);
            glTexCoord2f (0.0, 1.0);
            glVertex3f(0.0f,0.0f,-sizez);
        glEnd();

        glBindTexture(GL_TEXTURE_2D,texSkyBox[3].getId());
        glBegin(GL_QUADS);
            glNormal3f(0.0f,-1.0f,0.0f);
            glTexCoord2f (0.0, 0.0);
            glVertex3f(0.0f,sizey,-sizez);
            glTexCoord2f (1.0, 0.0);
            glVertex3f(sizex,sizey,-sizez);
            glTexCoord2f (1.0, 1.0);
            glVertex3f(sizex,sizey,0.0f);
            glTexCoord2f (0.0, 1.0);
            glVertex3f(0.0f,sizey,0.0f);
        glEnd();

        glBindTexture(GL_TEXTURE_2D,texSkyBox[0].getId());
	    glBegin(GL_QUADS);
	        //front face
            glNormal3f(0.0f,0.0f,-1.0f);
	        glTexCoord2f (0.0, 0.0);
            glVertex3f(sizex,0.0f,0.0f);
	        glTexCoord2f (1.0, 0.0);
	        glVertex3f(0.0f,0.0f,0.0f);  
	        glTexCoord2f (1.0, 1.0);
            glVertex3f(0.0f,sizey,0.0f);
	        glTexCoord2f (0.0, 1.0);
            glVertex3f(sizex,sizey,0.0f);
        glEnd();

        glBindTexture(GL_TEXTURE_2D,texSkyBox[1].getId());
	    glBegin(GL_QUADS);
	        //back face
            glNormal3f(0.0f,0.0f,1.0f);
	        glTexCoord2f (0.0, 0.0);
	        glVertex3f(0.0f, 0.0f,-sizez);
	        glTexCoord2f (1.0, 0.0);
	        glVertex3f(sizex,0.0f,-sizez);
	        glTexCoord2f (1.0, 1.0);
	        glVertex3f(sizex,sizey,-sizez);
	        glTexCoord2f (0.0, 1.0);
	        glVertex3f(0.0f,sizey,-sizez);
        glEnd();

        glBindTexture(GL_TEXTURE_2D,texSkyBox[4].getId());
	    glBegin(GL_QUADS);
	        //left face
            glNormal3f(1.0f,0.0f,0.0f);
	        glTexCoord2f (0.0, 0.0);
	        glVertex3f(0.0f,0.0f,0.0f);
	        glTexCoord2f (1.0, 0.0);
	        glVertex3f(0.0f,0.0f,-sizez);
	        glTexCoord2f (1.0, 1.0);
	        glVertex3f(0.0f,sizey,-sizez);
	        glTexCoord2f (0.0, 1.0);
	        glVertex3f(0.0f,sizey,0.0f);
        glEnd();

        glBindTexture(GL_TEXTURE_2D,texSkyBox[5].getId());
	    glBegin(GL_QUADS);
	        //right face
            glNormal3f(-1.0f,0.0f,0.0f);
	        glTexCoord2f (0.0, 0.0);
	        glVertex3f(sizex,0.0f,-sizez);
	        glTexCoord2f (1.0, 0.0);
	        glVertex3f(sizex,0.0f,0.0f);
	        glTexCoord2f (1.0, 1.0);
	        glVertex3f(sizex,sizey,0.0f);
	        glTexCoord2f (0.0, 1.0);
	        glVertex3f(sizex,sizey,-sizez);
	    glEnd();

        glTranslatef((sizex - boundingBox.width)/2.0f, 0.0f, -(sizez - boundingBox.depth)/2.0f);
        glDisable(GL_TEXTURE_2D);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //prevent z-fighting for street by moving it up a small amount
    glTranslatef(0.0f,epsilon,0.0f);

    //Show street
    if(showTexture)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,texStreet.getId());
        glBegin(GL_QUADS);
	        //right face
            glNormal3f(0.0f,1.0f,0.0f);
	        glTexCoord2f (0.0, 0.0);
	        glVertex3f(boundingBox.minx,boundingBox.miny,boundingBox.maxz);
	        glTexCoord2f (1.0, 0.0);
	        glVertex3f(boundingBox.maxx,boundingBox.miny,boundingBox.maxz);
	        glTexCoord2f (1.0, 1.0);
	        glVertex3f(boundingBox.maxx,boundingBox.miny,boundingBox.minz);
	        glTexCoord2f (0.0, 1.0);
	        glVertex3f(boundingBox.minx,boundingBox.miny,boundingBox.minz);
	    glEnd();
        glDisable(GL_TEXTURE_2D);
    }
    
    //Show model
    if(showModel)
    {
        glTranslatef(-20.f,0.0f,0.0f);
        //Display the input model.
        //The input model's edges.
        for(int numFaces = 0; numFaces < input3D.numFaces; ++numFaces)
        {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glLineWidth(1.4f);
            glBegin(GL_LINES);
            for(int numEdges = 0; numEdges < input3D.faces[numFaces].numEdges; ++numEdges)
            {
                glVertex3fv(input3D.faces[numFaces].edges[numEdges]->begin->val);
                glVertex3fv(input3D.faces[numFaces].edges[numEdges]->end->val);
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

        glTranslatef(20.0f,0.0f,0.0f);
    }

    //Draw the parallel faces.
    for(unsigned int x = 0; x < grid.parallelFaces.size(); x++)
    { 
        if(state.getTest() == x || state.getTest() == grid.parallelFaces.size() && state.getTest() != grid.parallelFaces.size() + 1)
        {
            for(unsigned int y = 0; y < grid.parallelFaces[x].size(); y++)
            {   
                glColor4fv(input3D.colors[x].val);
                glBegin(GL_POLYGON);
                for(int z = 0; z < grid.parallelFaces[x][y]->numEdges; z++)
                {
                    Vertex *vert = grid.parallelFaces[x][y]->edges[z]->begin;
                    glVertex3fv(grid.parallelFaces[x][y]->edges[z]->begin->val);
                }
                glEnd();
            }
        }
    }

    //Draw Vertices
    if(showGridVertices)
    {
        glEnable(GL_POINT_SMOOTH);  //Make the point a sphere basically.
        glPointSize(4.0f);          //Change the size of the point
        
        glBegin(GL_POINTS);
            for(int x=0; x < (int)grid.vertices.size(); x++)
            {
                if(grid.vertices[x]->connectedEdges == 3)
                {
                    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
                }
                else if(grid.vertices[x]->connectedEdges == 4)
                {
                    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                }
                else if(grid.vertices[x]->connectedEdges == 5)
                {
                    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
                }
                else if(grid.vertices[x]->connectedEdges == 6)
                {
                    glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
                }
                else
                {
                    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);  //should never get here, color red.
                }
                
                glVertex3fv(grid.vertices[x]->val);
            }
        glEnd();
        glDisable(GL_POINT_SMOOTH);
    }

    //Draw Edges
    if(showGridEdges)
    {
        glLineWidth(1.0f);
        for(int x = 0; x < (int)grid.edges.size(); x++)
        {
            glBegin(GL_LINES);
                glColor3f(1.0f,0.0f,0.0f);
                glVertex3fv(grid.edges[x]->begin->val);

                glColor3f(0.0f,0.0f,1.0f);
                glVertex3fv(grid.edges[x]->end->val);
            glEnd();
        }
    }

    //Draw Faces
    if(showGridFaces)
    {
        for(int x = 0; x < (int)grid.faces.size(); x++)
        {

            glBegin(GL_POLYGON);
            for(int z = 0; z < grid.faces[x]->numEdges; z++)
            {
                glColor4fv(input3D.colors[grid.faces[x]->set].val);
                glVertex3fv(grid.faces[x]->vertices[z]->val);
            }
            glEnd();
        }
    }

    //Draw Volumes
    if(showGridVolumes)
    {
        glColor3f(1.0f,1.0f,1.0f);
        glDisable(GL_BLEND);
        if(showTexture)
        {
            glEnable(GL_TEXTURE_2D);
        
        }
        for(int x = 0; x < (int)grid.volumes.size(); x++)
        {
            if(grid.volumes[x]->state != INTERIOR)
            {
                continue;
            }
            for(int y = 0; y < (int)grid.volumes[x]->numFaces; y++)
            {
                if(showTexture)
                {
               
                    if(grid.volumes[x]->faces[y]->normal == Vector(0.0f, 1.0f, 0.0f))
                    {
                        glBindTexture(GL_TEXTURE_2D,texCement.getId());
                    }
                    else
                    {
                        glBindTexture(GL_TEXTURE_2D,texBuilding.getId());
                    }
                }

                glBegin(GL_QUADS);
                    glNormal3f(grid.volumes[x]->faces[y]->normal.x,grid.volumes[x]->faces[y]->normal.y,grid.volumes[x]->faces[y]->normal.z);
                for(int z = 0; z < (int)grid.volumes[x]->faces[y]->numVertices; z++)
                {
                    
                    if(showTexture)
                    {

                        if(z == 0)
                        {
                            glTexCoord2f(0.0f,0.0f);
                        }
                        else if(z == 1)
                        {
                            glTexCoord2f(1.0f,0.0f);
                        }
                        else if(z == 2)
                        {
                            glTexCoord2f(1.0f,1.0f);
                        }
                        else if(z == 3)
                        {
                            glTexCoord2f(0.0f,1.0f);
                        }
                    }
                    else
                    {
                        glColor4fv(Utils::randColor(.5f).val);
                           glColor4f(((int)(grid.volumes[x]->faces[0]->edges[0]->begin->val[0]) % 50)/50.0f,
                             ((int)(grid.volumes[x]->faces[0]->edges[0]->begin->val[1]) % 50)/50.0f,
                             ((int)(grid.volumes[x]->faces[0]->edges[0]->begin->val[2]) % 50)/ -50.0f,
                             1.0);
                    }
                    
                    glVertex3fv(grid.volumes[x]->faces[y]->vertices[z]->val);
                    

                }
                glEnd();
                
            }
        }
        glDisable(GL_TEXTURE_2D);
    }
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
    cout << "---Initializing---" << endl;
    cout << "Loading textures...";
    state.setWireFrame(true);
    state.setLighting(false);
    state.setDrawLights(true);
    state.setDrawAxis(false);
    state.setPrintInfoOnScreen(true);
    state.setTest(4);

    texBuilding.loadTexture("windows.jpg");
    texCement.loadTexture("cement.jpg");
    texStreet.loadTexture("street.jpg");
    texSkyBox[0].loadTexture("greenhill_negative_z.png");
    texSkyBox[1].loadTexture("greenhill_positive_z.png");
    texSkyBox[2].loadTexture("greenhill_negative_y.png");
    texSkyBox[3].loadTexture("greenhill_positive_y.png");
    texSkyBox[4].loadTexture("greenhill_negative_x.png");
    texSkyBox[5].loadTexture("greenhill_positive_x.png");

    texSkyBox[0].uploadTexture();
    texSkyBox[1].uploadTexture();
    texSkyBox[2].uploadTexture();
    texSkyBox[3].uploadTexture();
    texSkyBox[4].uploadTexture();
    texSkyBox[5].uploadTexture();
    texStreet.uploadTexture();
    texBuilding.uploadTexture();
    texCement.uploadTexture();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, 0);
    cout << "done" << endl;
    
    //unsigned int seed = (unsigned int)time(NULL);
    unsigned int seed = 1290455743;
    srand(seed);

    cout << "Parsing model files...";
	//Bounding box info
	int bbnumVertices;
	Vertex* bbvertices = NULL;  //Should be deleted in boundingbox.init
	//parser.boundingBox("tetrahedron.model", bbnumVertices, bbvertices);
    parser.boundingBox("cube.model", bbnumVertices, bbvertices);
    boundingBox.init(bbnumVertices, bbvertices);

	//Vertex info: Must be specified in a counter-clockwise order
	int numVertices;
	int numFaces;
	Face* faces = NULL;
	Vertex *vertices = NULL;
	//parser.vertexArray("tetrahedron.model", numVertices, vertices, numFaces, faces);
    parser.vertexArray("cube.model", numVertices, vertices, numFaces, faces);

	int numVerticesCube;
	int numFacesCube;
	Face* facesCube = NULL;
	Vertex *verticesCube = NULL;
	//parser.vertexArray("tetrahedron.model", numVerticesCube, verticesCube, numFacesCube, facesCube);*/
    parser.vertexArray("cube.model", numVerticesCube, verticesCube, numFacesCube, facesCube);

    cout << "done" << endl;
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
		cube.init(numVerticesCube, verticesCube, numFacesCube, facesCube);
        input3D.init(numVertices, vertices, numFaces, faces);

        grid.init(cube, boundingBox);
        //grid.init(input3D,boundingBox);
        continuousModelSynthesis3D(grid.edges, grid.vertices);
    }
}