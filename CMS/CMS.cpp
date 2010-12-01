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
 showGridVolumes(true),
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
    //Display the bounding box
    glTranslatef(12.8f, 11.6f, 0.0f);
    glScalef(.46f, .46f, .46f);
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

    if(showTexture)
    {
            for(int x = 0; x < boundingBox.numFaces; x++)
            {
                if(boundingBox.faces[x].normal == Vector(0.0f,1.0f,0.0f))
                {
                    glEnable(GL_TEXTURE_2D);
                glBegin(GL_QUADS);
                glNormal3f(boundingBox.faces[x].normal.x,boundingBox.faces[x].normal.y,boundingBox.faces[x].normal.z);
                for(int y = 0; y < boundingBox.faces[x].numVertices; y++)
                {
                    glVertex3fv((*boundingBox.faces[x].vertices[y]).val);
                }
                glEnd();
                glDisable(GL_TEXTURE_2D);
                break;
                }
            }
    }

	/*ilBindImage(texids[0]);
    glGenTextures(1, &texture); 
    glBindTexture(GL_TEXTURE_2D, texture); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
      ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
      ilGetData());*/

    //Grass
    if(showTexture)
    {
        glEnable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        float size = 100.0f;
        glTranslatef(0.0f, size, 0.0f);
	    glBindTexture(GL_TEXTURE_2D,texGrass.getId());
	
	    glBegin(GL_QUADS);
            glNormal3f(1.0f,1.0f,1.0f);
	        glTexCoord2f (0.0, 0.0);
	        glVertex3f(-size,-size,size);
	        glTexCoord2f (50.0, 0.0);
	        glVertex3f(-size,-size,-size);
	        glTexCoord2f (50.0, 50.0);
	        glVertex3f(size,-size,-size);
	        glTexCoord2f (0.0, 50.0);
	        glVertex3f(size,-size,size);
	    glEnd();


	/*ilBindImage(texids[1]);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
      ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
      ilGetData());

	glBindTexture(GL_TEXTURE_2D, clouds.getId());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,clouds.getWidth(), clouds.getHeight(), 0, clouds.getFormat(), GL_UNSIGNED_BYTE, clouds.getData());
*/
    glBindTexture(GL_TEXTURE_2D,texCloudsSide.getId());
	glBegin(GL_QUADS);
	    //front face
        glNormal3f(0.0f,0.0f,-1.0f);
	    glTexCoord2f (0.0, 0.0);
	    glVertex3f(-size,-size,size);  
	    glTexCoord2f (1.0, 0.0);
	    glVertex3f(size,-size,size);
	    glTexCoord2f (1.0, 1.0);
	    glVertex3f(size,size,size);
	    glTexCoord2f (0.0, 1.0);
	    glVertex3f(-size,size,size);
	    //back face
        glNormal3f(0.0f,0.0f,1.0f);
	    glTexCoord2f (0.0, 0.0);
	    glVertex3f(-size,-size,-size);
	    glTexCoord2f (1.0, 0.0);
	    glVertex3f(size,-size,-size);
	    glTexCoord2f (1.0, 1.0);
	    glVertex3f(size,size,-size);
	    glTexCoord2f (0.0, 1.0);
	    glVertex3f(-size,size,-size);
	    //left face
	    glTexCoord2f (0.0, 0.0);
	    glVertex3f(-size,-size,size);
	    glTexCoord2f (1.0, 0.0);
	    glVertex3f(-size,-size,-size);
	    glTexCoord2f (1.0, 1.0);
	    glVertex3f(-size,size,-size);
	    glTexCoord2f (0.0, 1.0);
	    glVertex3f(-size,size,size);
	    //right face
	    glTexCoord2f (0.0, 0.0);
	    glVertex3f(size,-size,size);
	    glTexCoord2f (1.0, 0.0);
	    glVertex3f(size,-size,-size);
	    glTexCoord2f (1.0, 1.0);
	    glVertex3f(size,size,-size);
	    glTexCoord2f (0.0, 1.0);
	    glVertex3f(size,size,size);
	glEnd();

        glTranslatef(0.0f, -size, 0.0f);
        glDisable(GL_TEXTURE_2D);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDisable(GL_TEXTURE_2D);
	//glDeleteTextures(1, &texture);
/*
	ilBindImage(texids[2]);
    glGenTextures(1, &texture); 
    glBindTexture(GL_TEXTURE_2D, texture); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
      ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
      ilGetData());
	glBegin(GL_QUADS);
	//top face
	glTexCoord2f (0.0, 0.0);
//	glBegin(GL_QUADS);
	glVertex3f(size,size,size);
	glTexCoord2f (1.0, 0.0);
	glVertex3f(-size,size,size);
	glTexCoord2f (1.0, 1.0);
	glVertex3f(-size,size,-size);
	glTexCoord2f (0.0, 1.0);
	glVertex3f(size,size,-size);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDeleteTextures(1, &texture);
	*/
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
    }

    //Draw the parallel faces.
    glTranslatef(20.0f,0.0f,0.0f);
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
    /*for(int x = 0; x < grid.vertices.size(); x++)
    {
        if(grid.vertices[x]->connectedEdges == 3)
        {
            glEnable(GL_POINT_SMOOTH);  //Make the point a sphere basically.
            glPointSize(4.0f);          //Change the size of the point
            glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
            glBegin(GL_POINTS);
            glVertex3fv(grid.vertices[x]->val);
            glEnd();
            glDisable(GL_POINT_SMOOTH);
        }
        else if(grid.vertices[x]->connectedEdges == 4)
        {
            glEnable(GL_POINT_SMOOTH);  //Make the point a sphere basically.
            glPointSize(4.0f);          //Change the size of the point
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            glBegin(GL_POINTS);
            glVertex3fv(grid.vertices[x]->val);
            glEnd();
            glDisable(GL_POINT_SMOOTH);
        }
        else if(grid.vertices[x]->connectedEdges == 5)
        {
            glEnable(GL_POINT_SMOOTH);  //Make the point a sphere basically.
            glPointSize(4.0f);          //Change the size of the point
            glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
            glBegin(GL_POINTS);
            glVertex3fv(grid.vertices[x]->val);
            glEnd();
            glDisable(GL_POINT_SMOOTH);
        }
        else if(grid.vertices[x]->connectedEdges == 6)
        {
            glEnable(GL_POINT_SMOOTH);  //Make the point a sphere basically.
            glPointSize(4.0f);          //Change the size of the point
            glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
            glBegin(GL_POINTS);
            glVertex3fv(grid.vertices[x]->val);
            glEnd();
            glDisable(GL_POINT_SMOOTH);
        }
    }*/

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
               
                if(grid.volumes[x]->faces[y]->normal == Vector(0.0f, 1.0f, 0.0f))
                {
                    glBindTexture(GL_TEXTURE_2D,texCement.getId());
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D,texBuilding.getId());
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
    state.setWireFrame(true);
    state.setLighting(false);
    state.setDrawLights(true);
    state.setDrawAxis(false);
    state.setPrintInfoOnScreen(true);
    state.setTest(4);

    texCloudsTop.loadTexture("cloudst.jpg");
    texCloudsSide.loadTexture("clouds.jpg");
    texGrass.loadTexture("grass2.jpg");
    texBuilding.loadTexture("windows3.jpg");
    texCement.loadTexture("cement.jpg");

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glGenTextures(1, texCloudsTop.setId());
    glBindTexture(GL_TEXTURE_2D, texCloudsTop.getId()); 
    glTexImage2D(GL_TEXTURE_2D, 0, texCloudsTop.getFormat(),texCloudsTop.getWidth(),texCloudsTop.getHeight(), 0, texCloudsTop.getFormat(), GL_UNSIGNED_BYTE, texCloudsTop.getData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST ); 
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, texCloudsTop.getWidth(), texCloudsTop.getHeight(), texCloudsTop.getFormat(), GL_UNSIGNED_BYTE, texCloudsTop.getData() );
    texCloudsTop.destroyTexture();

    glGenTextures(1, texCloudsSide.setId());
    glBindTexture(GL_TEXTURE_2D, texCloudsSide.getId()); 
    glTexImage2D(GL_TEXTURE_2D, 0, texCloudsSide.getFormat(),texCloudsSide.getWidth(),texCloudsSide.getHeight(), 0, texCloudsSide.getFormat(), GL_UNSIGNED_BYTE, texCloudsSide.getData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST ); 
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, texCloudsSide.getWidth(), texCloudsSide.getHeight(), texCloudsSide.getFormat(), GL_UNSIGNED_BYTE, texCloudsSide.getData() );
    texCloudsSide.destroyTexture();

    glGenTextures(1, texGrass.setId());
    glBindTexture(GL_TEXTURE_2D, texGrass.getId()); 
    glTexImage2D(GL_TEXTURE_2D, 0, texGrass.getFormat(),texGrass.getWidth(),texGrass.getHeight(), 0, texGrass.getFormat(), GL_UNSIGNED_BYTE, texGrass.getData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, texGrass.getWidth(), texGrass.getHeight(), texGrass.getFormat(), GL_UNSIGNED_BYTE, texGrass.getData() );
    texGrass.destroyTexture();

    glGenTextures(1, texBuilding.setId());
    glBindTexture(GL_TEXTURE_2D, texBuilding.getId()); 
    glTexImage2D(GL_TEXTURE_2D, 0, texBuilding.getFormat(),texBuilding.getWidth(),texBuilding.getHeight(), 0, texBuilding.getFormat(), GL_UNSIGNED_BYTE, texBuilding.getData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, texBuilding.getWidth(), texBuilding.getHeight(), texBuilding.getFormat(), GL_UNSIGNED_BYTE, texBuilding.getData() );
    texBuilding.destroyTexture();

    glGenTextures(1, texCement.setId());
    glBindTexture(GL_TEXTURE_2D, texCement.getId()); 
    glTexImage2D(GL_TEXTURE_2D, 0, texCement.getFormat(),texCement.getWidth(),texCement.getHeight(), 0, texCement.getFormat(), GL_UNSIGNED_BYTE, texCement.getData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, texCement.getWidth(), texCement.getHeight(), texCement.getFormat(), GL_UNSIGNED_BYTE, texCement.getData() );
    texCement.destroyTexture();

    glBindTexture(GL_TEXTURE_2D, 0);

    lights.addLight(250,250.0f,-250.0f, 0.0f);
    //glDisable(GL_TEXTURE_2D);

    
    //unsigned int seed = (unsigned int)time(NULL);
    unsigned int seed = 1290455743;
    srand(seed);
    cout << "Seed value: " << seed << endl;

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
        continuousModelSynthesis3D(grid.edges, grid.vertices);
    }
}