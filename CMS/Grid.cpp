/**
    \file Grid.cpp
    \brief 
*/

#include "CMSModel.h"
#include "Grid.h"
#include "Geometry.h"
#include "Utilities.h"
#include <iostream>
#include <vector>



void Grid::init(const CMSModel3D &model, const Cuboid &boundingbox)
{
    //Go through each face of the model, and look at the normal.
    for(int x = 0; x < model.numFaces; x++)
    {
        //Extract the normal from the face.
        Vector normal = model.faces[x].normal;

        //Figure out what planes of the bounding box that normal can intersect.
        
        
    }
}

void Grid::init(const CMSModel &model, const Cuboid &boundingbox)
{
    enum SlopeType {VERTICAL = 0, HORIZONTAL, POSITIVE, NEGATIVE, UNASSIGNED};
    float topy = boundingbox.edges[TOP].end->val[Y];
    float bottomy = boundingbox.edges[BOTTOM].end->val[Y];
    float rightx = boundingbox.edges[RIGHT].end->val[X];
    float leftx = boundingbox.edges[LEFT].end->val[X];

    float     currenty  = 0.0;
    float     yincrement = 0.0;
    SlopeType slopetype = UNASSIGNED;
    numModelEdges = model.numEdges;
    parallelEdges = new Edge*[model.numEdges];
    numEdges      = new int[model.numEdges];

    //Find a random spacing between [maximumLengthVertex/2.0,maximumLengthVertex]
    float max = 0.0f;
    for(int x = 0; x < model.numVertices; x++)
    {
        float length = sqrt(model.vertices[x].val[X]*model.vertices[x].val[X] + model.vertices[x].val[Y]*model.vertices[x].val[Y]);
        if(length > max)
        {
            max = length;
        }
    }
    spacing = Utils::randFloat(max/2.0f, max);

    for(int set = 0; set < model.numEdges; ++set)
    {
        Fraction slope = model.edges[set].edgestate.slope;
        if(fabs(slope.den - 0.0) < EPSILON)
        {
            slopetype = VERTICAL;
            currenty = 0.0f;
        }
        else if(fabs(slope.num - 0.0) < EPSILON)
        {
            slopetype = HORIZONTAL;
            currenty = 0.0f+Utils::randFloat(0.0f, spacing/2.0f);
            yincrement = spacing;
        }
        else if(slope.value > 0.0)
        {
            slopetype = POSITIVE;
            currenty = -slope.value*rightx+slope.value*leftx+bottomy+Utils::randFloat(0.0f, spacing/2.0f);
            yincrement = fabs(spacing/sin(atan(1.0f/slope.value)));
        }
        else
        {
            slopetype = NEGATIVE;
            currenty = -slope.value*leftx+slope.value*rightx+bottomy+Utils::randFloat(0.0f, spacing/2.0f);
            yincrement = fabs(spacing/sin(atan(-1.0f/slope.value)));
        }

        //TODO: Only allocate for DISTINCT edges
        int maxNumEdges    = int(topy-currenty/float(spacing));
        parallelEdges[set] = new Edge[maxNumEdges];
        numEdges[set]      = maxNumEdges;
        int edgeCount      = 0;
        while(currenty <= topy)
        {
            if(slopetype == POSITIVE)   //This means it can only intersect TOP or RIGHT of the bounding box
            {
                //Check to see if it intersects TOP
                float tempx = ((topy-currenty)+slope.value*leftx)/slope.value;
                if(tempx < rightx)
                {
                    Vertex *begin = new Vertex(leftx, currenty);
                    Vertex *end  = new Vertex(tempx, topy);
                    parallelEdges[set][edgeCount] = Edge(begin, end);
                    edgeCount++;
                }
                //Check to see if it intersects RIGHT
                else
                {
                    float tempy = slope.value*rightx-slope.value*0+currenty;
                    if(tempy >= bottomy)
                    {
                        Vertex *begin;
                        if(currenty < 0)
                        {
                            float tempx2 = ((bottomy-currenty)+slope.value*leftx)/slope.value;
                            begin = new Vertex(tempx2, bottomy);
                        }
                        else
                        {
                            begin = new Vertex(leftx, currenty);
                        }
                        Vertex *end  = new Vertex(rightx, tempy);
                        parallelEdges[set][edgeCount] = Edge(begin, end);
                        edgeCount++;
                    }
                }
            }
            else if(slopetype == NEGATIVE) //Check to see if it intersects TOP OR LEFT of the bounding box
            {
                //First see if it intersects TOP
                float tempx = ((topy-currenty)+slope.value*rightx)/slope.value;
                if(tempx > leftx)
                {
                    Vertex *begin = new Vertex(rightx, currenty);
                    Vertex *end  = new Vertex(tempx, topy);
                    parallelEdges[set][edgeCount] = Edge(begin, end);
                    edgeCount++;
                }
                else
                {
                    //See if it intersects LEFT
                    float tempy = slope.value*leftx-slope.value*rightx+currenty;
                    if(tempy >= bottomy)
                    {
                        Vertex *begin;
                        if(currenty < 0)
                        {
                            float tempx2 = ((bottomy-currenty)+slope.value*rightx)/slope.value;
                            begin = new Vertex(tempx2, bottomy);
                        }
                        else
                        {
                            begin = new Vertex(rightx, currenty);
                        }
                        Vertex *end  = new Vertex(leftx, tempy);
                        parallelEdges[set][edgeCount] = Edge(begin, end);
                        edgeCount++;
                    }
                }
            }
            else if(slopetype == HORIZONTAL)
            {
                Vertex *begin = new Vertex(leftx, currenty);
                Vertex *end  = new Vertex(rightx, currenty);
                parallelEdges[set][edgeCount] = Edge(begin, end);
                edgeCount++;
            }
            else if(slopetype == VERTICAL)
            {
                //TODO: Implement vertical case
                std::cout << "ERROR: Need to implement vertical case." << std::endl;
                exit(EXIT_FAILURE);
            }
            else
            {
                std::cout << "ERROR: Invalid slope type." << std::endl;
                exit(EXIT_FAILURE);
            }
            currenty += yincrement;
        }
        numEdges[set] = edgeCount;
    }

    //Find vertex intersections
    for(int x = 0; x < model.numEdges; x++)
    {
        for(int y = 0; y < numEdges[x]; y++)
        {
            int start = 0;
            for(int i = x+1; i < model.numEdges; i++)
            {
                for(int j = 0; j < numEdges[i]; j++)
                {
                    Vertex *temp = NULL;
                    if(parallelEdges[x][y].intersect(parallelEdges[i][j],temp) == INTERESECTING)
                    {
                        vertices.push_back(temp);
                        start++;
                    }
                }
            }
        }
    }

    //Separate the edges, assign vertices to edges, and edges to vertices.
    int currentEdge = 0;
    for(int x = 0; x < model.numVertices; x++)
    {
        for(int y = 0; y < numEdges[x]; y++)
        {
            vector<Vertex*> verticesOnEdge;
            Vertex *vert = parallelEdges[x][y].begin;
            float slope = parallelEdges[x][y].edgestate.slope.value;
            for(unsigned int i = 0; i < vertices.size(); i++)
            {
                //y-y1 = m(x-x1)
                if(fabs((vert->val[Y]-vertices[i]->val[Y]) - (slope*vert->val[X] - slope*vertices[i]->val[X])) < EPSILON)
                {
                    verticesOnEdge.push_back(vertices[i]);
                }
            }

            //We don't want to include any edges that only have one vertex
            if(verticesOnEdge.size() > 1)
            {
                sortVertices(verticesOnEdge,0,verticesOnEdge.size()-1);                

#ifdef _DEBUG
                //Sanity check: Make sure the vertices are in order.
                for(int a = 0; a < (int)verticesOnEdge.size()-1; a++)
                {
                    if(!(verticesOnEdge[a]->val[X] < verticesOnEdge[a+1]->val[X]))
                    {
                        cout << "SANITY ERROR: Vertices are not sorted." << endl;
                        cout << verticesOnEdge[a]->val[X] << " " <<  verticesOnEdge[a+1]->val[X] << endl;
                        exit(EXIT_FAILURE);
                    }
                }
#endif

                for(unsigned int j = 0; j < verticesOnEdge.size()-1; j++)
                {
                    Edge *edge = new Edge(verticesOnEdge[j],verticesOnEdge[j+1]);
                    edge->edgestate.set = x;
                    edges.push_back(edge);

                    verticesOnEdge[j]->edges[verticesOnEdge[j]->connectedEdges] = edge;
                    verticesOnEdge[j+1]->edges[verticesOnEdge[j+1]->connectedEdges] = edge;
                    verticesOnEdge[j]->connectedEdges++;
                    verticesOnEdge[j+1]->connectedEdges++;
                } 
            }
        }
    }

    //Find if a vertex only has one edge.
    int x = 0;
    for(vector<Vertex*>::iterator vertIter = vertices.begin(); vertIter < vertices.end();)
    {
        
        if((*vertIter)->connectedEdges == 1)
        {
            for(vector<Edge*>::iterator edgeIter = edges.begin(); edgeIter < edges.end(); edgeIter++)
            {
                //Find the edge in 'edges' that corresponds to the one edge in vertices.
                if(((*edgeIter) != NULL) && ((*edgeIter) == (*vertIter)->edges[0]) )
                {
                    //Make sure that we decrement the number of edges of the other vertex it is connected to,
                    //Then make the pointer to that edge go to null
                    Vertex *setToNull;
                    int z = 0;
                    if(((*vertIter) != NULL) && ((*edgeIter)->begin == (*vertIter)))
                    {
                        (*edgeIter)->end->connectedEdges--;
                        for(z = 0;  z < 4; z++)
                        {
                            if((*edgeIter) == (*edgeIter)->end->edges[z])
                            {
                                setToNull = (*edgeIter)->end;
                                break;
                            }
                        }
                    }
                    else
                    {
                        (*edgeIter)->begin->connectedEdges--;
                        
                        for(z = 0;  z < 4; z++)
                        {
                            if((*edgeIter) == (*edgeIter)->begin->edges[z])
                            {
                                setToNull = (*edgeIter)->begin;
                                break;
                            }
                        }
                    }

                    delete (*edgeIter);
                    (*edgeIter) = NULL;
                    edgeIter = edges.erase(edgeIter);

                    delete (*vertIter);
                    (*vertIter) = NULL;
                    vertIter = vertices.erase(vertIter);                         

                    setToNull->edges[z] = NULL;

                    break;
                }
            }
        }
        else
        {
            vertIter++;
        }
    }

#ifdef _DEBUG
    //Sanity check: Make sure all currentEdge counts are correct and everything else is null.
    //Sanity check: Make sure all vertices have more than 1 edge.
    for(int x = 0; x < (int)vertices.size(); x++)
    {
        int count = 0;
        for(int y = 0; y < 4; y++)
        {

            if(vertices[x]->edges[y] != NULL)
            {
                count++;
            }
        }
        if(count != vertices[x]->connectedEdges)
        {
            cout << "SANITY ERROR: currentEdge count is incorrect for some vertices (or some edges don't point to NULL like they should" << endl;
            exit(EXIT_FAILURE);
        }

        if(count == 0 || count == 1)
        {
            cout << "SANITY ERROR: Vertex has one or no edges." << endl;
            exit(EXIT_FAILURE);
        }
    }

    //Sanity check: Make sure the values for vertex val are reasonable.
    for(int x = 0; x < (int)edges.size(); x++)
    {
        if(edges[x]->begin->val[Y] > topy || edges[x]->begin->val[Y] < bottomy)
        {
            cout << "SANITY ERROR: Vertex values unreasonable - begin, y value" << endl;
            exit(EXIT_FAILURE);
        }
        else if(edges[x]->begin->val[X] > rightx || edges[x]->begin->val[X] < leftx)
        {
            cout << "SANITY ERROR: Vertex values unreasonable - begin, x value" << endl;
            exit(EXIT_FAILURE);
        }
        else if(edges[x]->end->val[Y] > topy || edges[x]->end->val[Y] < bottomy)
        {
            cout << "SANITY ERROR: Vertex values unreasonable - end, y value" << endl;
            exit(EXIT_FAILURE);
        }
        else if(edges[x]->end->val[X] > rightx || edges[x]->end->val[X] < leftx)
        {
            cout << "SANITY ERROR: Vertex values unreasonable - end, x value" << endl;
            exit(EXIT_FAILURE);
        }
    }
#endif
}

void Grid::sortVertices(vector<Vertex*> &vertices, int left, int right)
{
    if(left>=right)
        return;
    int middle = left;
    for(int i = left+1; i <= right; i++){
        if(vertices[i]->val[X] < vertices[left]->val[X])
        {
            Vertex *temp = vertices[++middle];
            vertices[middle] = vertices[i];
            vertices[i] = temp;  
        }
    }
    Vertex *temp = vertices[left];
    vertices[left] = vertices[middle];
    vertices[middle] = temp;
    sortVertices(vertices,left,middle-1);
    sortVertices(vertices,middle+1,right);
}

Grid::Grid()
    :spacing(4.0f)     //TODO: For spacing, maybe find max distance between all points then times .75?
{

}

Grid::~Grid()
{
    for(int x = 0;  x < numModelEdges; x++)
    {
        delete [] parallelEdges[x];
    }

    delete [] parallelEdges;
    delete [] numEdges;

    for(unsigned int x = 0; x < vertices.size(); x++)
    {
        delete vertices[x];
    }
    vertices.clear();

    for(unsigned int x = 0; x < edges.size(); x++)
    {
        delete edges[x];
    }
    edges.clear();
}