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

void Grid::init(const CMSModel &model, const Cuboid &boundingbox)
{
    enum SlopeType {VERTICAL = 0, HORIZONTAL, POSITIVE, NEGATIVE, UNASSIGNED};
    float topy = boundingbox.edges[TOP].end->val[Y];
    float bottomy = boundingbox.edges[BOTTOM].end->val[Y];
    float rightx = boundingbox.edges[RIGHT].end->val[X];
    float leftx = boundingbox.edges[LEFT].end->val[X];

    float currenty = 0.0;
    SlopeType slopetype = UNASSIGNED;
    parallelEdges = new Edge*[model.numVerticies];
    float yincrement = 0.0;
    
    for(int edgeNum = 0; edgeNum < model.numVerticies; ++edgeNum)
    {
        Fraction slope = model.edges[edgeNum].edgestate.slope;
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
        int numberofedges = int(topy-currenty/float(spacing));
        parallelEdges[edgeNum] = new Edge[numberofedges];
        numEdges[edgeNum] = numberofedges;
        int currentedge = 0;
        while(currenty <= topy)
        {
            if(slopetype == POSITIVE)   //This means it can only intersect TOP or RIGHT
            {
                //Check to see if it intersects TOP
                float tempx = ((topy-currenty)+slope.value*leftx)/slope.value;
                if(tempx < rightx)
                {
                    Vertex *begin = new Vertex(leftx, currenty);
                    Vertex *end  = new Vertex(tempx, topy);
                    //Edge *edge = new Edge(begin, end);
                    parallelEdges[edgeNum][currentedge] = Edge(begin, end);
                    currentedge++;
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
                        //Edge *edge = new Edge(begin, end);
                        parallelEdges[edgeNum][currentedge] = Edge(begin, end);
                        currentedge++;
                    }
                }
            }
            else if(slopetype == NEGATIVE) //Check to see if it intersects TOP OR LEFT
            {
                //First see if it intersects TOP
                float tempx = ((topy-currenty)+slope.value*rightx)/slope.value;
                if(tempx > leftx)
                {
                    Vertex *begin = new Vertex(rightx, currenty);
                    Vertex *end  = new Vertex(tempx, topy);
                    //Edge *edge = new Edge(begin, end);
                    parallelEdges[edgeNum][currentedge] = Edge(begin, end);
                    currentedge++;
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
                        //Edge *edge = new Edge(begin, end);
                        parallelEdges[edgeNum][currentedge] = Edge(begin, end);
                        currentedge++;
                    }
                }
            }
            else if(slopetype == HORIZONTAL)
            {
                Vertex *begin = new Vertex(leftx, currenty);
                Vertex *end  = new Vertex(rightx, currenty);
                parallelEdges[edgeNum][currentedge] = Edge(begin, end);
                currentedge++;
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
        numEdges[edgeNum] = currentedge;

    }

    int currentVert = 0;

    //Find vertex intersections
    for(int x = 0; x < model.numVerticies; x++)
    {
        for(int y = 0; y < numEdges[x]; y++)
        {
            int start = 0;
            for(int i = x+1; i < model.numVerticies; i++)
            {
                for(int j = 0; j < numEdges[i]; j++)
                {
                    Vertex *temp = NULL;
                    if(parallelEdges[x][y].intersect(parallelEdges[i][j],temp) == INTERESECTING)
                    {
                        verticies.push_back(temp);
                        currentVert++;
                        start++;
                    }
                }
            }
            //Need to make sure we only include verticies that are on an edge with more than one intersection, or odd results will follow.
            if(start == 1)
            {
                verticies.erase(verticies.end()-1);
                currentVert--;
            }
        }
    }
    totalVerts = currentVert;

    //BRUTE FORCE
    //Separate the edges, assign verticies to edges, and edges to verticies.
    int currentEdge = 0;
    for(int x = 0; x < model.numVerticies; x++)
    {
        for(int y = 0; y < numEdges[x]; y++)
        {
            vector<Vertex*> verticiesOnEdge;
            Vertex *vert = parallelEdges[x][y].begin;
            float slope = parallelEdges[x][y].edgestate.slope.value;
            for(int i = 0; i < (int)verticies.size(); i++)
            {
                //y-y1 = m(x-x1)
                if(fabs((vert->val[Y]-verticies[i]->val[Y]) - (slope*vert->val[X] - slope*verticies[i]->val[X])) < EPSILON)
                {
                    verticiesOnEdge.push_back(verticies[i]);
                }
            }

            if((int)verticiesOnEdge.size() > 1)
            {

                sortVerticies(verticiesOnEdge,0,verticiesOnEdge.size()-1);                
                
#ifdef _DEBUG
                //Sanity check: Make sure the verticies are in order.
                for(int a = 0; a < (int)verticiesOnEdge.size()-1; a++)
                {
                    if(!(verticiesOnEdge[a]->val[X] < verticiesOnEdge[a+1]->val[X]))
                    {
                        cout << "SANITY ERROR: Verticies are not sorted." << endl;
                        cout << verticiesOnEdge[a]->val[X] << " " <<  verticiesOnEdge[a+1]->val[X] << endl;
                        exit(EXIT_FAILURE);
                    }
                }
#endif
                
                for(int j = 0; j < (int)verticiesOnEdge.size()-1; j++)
                {
                    Edge *edge = new Edge(verticiesOnEdge[j],verticiesOnEdge[j+1]);
                    edge->edgestate.set = x;
                    edges.push_back(edge);

                    verticiesOnEdge[j]->edges[verticiesOnEdge[j]->connectedEdges] = edge;
                    verticiesOnEdge[j+1]->edges[verticiesOnEdge[j+1]->connectedEdges] = edge;
                    verticiesOnEdge[j]->connectedEdges++;
                    verticiesOnEdge[j+1]->connectedEdges++;
                } 
            }
        }
    }

    //Find if a vertex only has one edge.
    for(int x = 0; x < (int)verticies.size(); x++)
    {
        if(verticies[x]->connectedEdges == 1)
        {
            for(int y = 0; y < (int)edges.size(); y++)
            {
                //Find the edge in 'edges' that corresponds to the one edge in verticies.
                if((edges[y] != NULL) && (edges[y] == verticies[x]->edges[0]) )
                {
                    //Make sure that we decrement the number of edges of the other vertex it is connected to,
                    //Then make the pointer to that edge go to null
                    Vertex *setToNull;
                    int z = 0;
                    if((verticies[x] != NULL) && (edges[y]->begin == verticies[x]))
                    {

                        edges[y]->end->connectedEdges--;
                        for(z = 0;  z < 4; z++)
                        {
                            if(edges[y] == edges[y]->end->edges[z])
                            {
                                setToNull = edges[y]->end;
                                break;
                            }
                        }
                    }
                    else
                    {
                        edges[y]->begin->connectedEdges--;
                        for(z = 0;  z < 4; z++)
                        {
                            if(edges[y] == edges[y]->begin->edges[z])
                            {
                                setToNull = edges[y]->begin;
                                break;
                            }
                        }
                    }
                    delete edges[y];
                    edges[y] = NULL;
                    edges.erase(edges.begin()+y);

                    delete verticies[x];
                    verticies[x] = NULL;
                    verticies.erase(verticies.begin()+x);
                
                    setToNull->edges[z] = NULL;

                    break;
                }
            }
        }
    }

#ifdef _DEBUG
    //Sanity check: Make sure all currentEdge counts are correct and everything else is null.
    for(int x = 0; x < (int)verticies.size(); x++)
    {
        int count = 0;
        for(int y = 0; y < 4; y++)
        {
            
            if(verticies[x]->edges[y] != NULL)
            {
                count++;
            }
        }
        if(count != verticies[x]->connectedEdges)
        {
            cout << "SANITY ERROR: currentEdge count is incorrect for some verticies (or some edges don't point to NULL like they should" << endl;
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

void Grid::sortVerticies(vector<Vertex*> &verticies, int left, int right)
{
    if(left>=right)
        return;
    int middle = left;
    for(int i = left+1; i <= right; i++){
        if(verticies[i]->val[X] < verticies[left]->val[X])
        {
            Vertex *temp = verticies[++middle];
            verticies[middle] = verticies[i];
            verticies[i] = temp;  
        }
    }
    Vertex *temp = verticies[left];
    verticies[left] = verticies[middle];
    verticies[middle] = temp;
    sortVerticies(verticies,left,middle-1);
    sortVerticies(verticies,middle+1,right);
}