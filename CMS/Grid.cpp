
#include "CMSModel.h"
#include "Grid.h"
#include "Geometry.h"
#include "Utilities.h"
#include <iostream>
#include <vector>

void Grid::init(const CMSModel &model, const Cuboid &boundingbox)
{
    enum SlopeType {VERTICAL = 0, HORIZONTAL, POSITIVE, NEGATIVE};
    float topy = boundingbox.edges[TOP].end->val[Y];
    float bottomy = boundingbox.edges[BOTTOM].end->val[Y];
    float rightx = boundingbox.edges[RIGHT].end->val[X];
    float leftx = boundingbox.edges[LEFT].end->val[X];
    float currenty;
    SlopeType slopetype;
    parallelEdges = new Edge*[model._numVerticies];
    
    for(int edgeNum = 0; edgeNum < model._numVerticies; ++edgeNum)
    {
        Fraction slope = model._edges[edgeNum].edgestate.slope;
        if(fabs(slope.den - 0.0) < EPSILON)
        {
            slopetype = VERTICAL;
            currenty = 0.0f;
        }
        else if(fabs(slope.num - 0.0) < EPSILON)
        {
            slopetype = HORIZONTAL;
            currenty = 0.0f+Utils::randFloat(0.0f, spacing);
        }
        else if(slope.value > 0.0)
        {
            slopetype = POSITIVE;
            currenty = -slope.value*rightx+slope.value*leftx+bottomy+Utils::randFloat(0.0f, spacing);
        }
        else
        {
            slopetype = NEGATIVE;
            currenty = -slope.value*leftx+slope.value*rightx+bottomy+Utils::randFloat(0.0f, spacing);
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
                    Vertex *begin = new Vertex(0.0f, currenty);
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
            currenty += spacing;
        }
        numEdges[edgeNum] = currentedge;

    }

   // totalVerts = 1;
    //for(int x = 0; x < model._numVerticies; x++)
    //{
    //    totalVerts *= numEdges[x];
    //}
    //verticies = new Vertex[totalVerts];
    int currentVert = 0;

    //Find vertex intersections
    for(int x = 0; x < model._numVerticies; x++)
    {
        for(int y = 0; y < numEdges[x]; y++)
        {
            for(int i = x+1; i < model._numVerticies; i++)
            {
                for(int j = 0; j < numEdges[i]; j++)
                {
                    Vertex *temp = new Vertex();
                    if(parallelEdges[x][y].intersect(parallelEdges[i][j],temp) == INTERESECTING)
                    {
                        verticies.push_back(temp);
                        currentVert++;
                    }
                }
            }
        }
    }
    totalVerts = currentVert;

    //BRUTE FORCE
    //Separate the edges, assign verticies to edges, and edges to verticies.
    int currentEdge = 0;
    for(int x = 0; x < model._numVerticies; x++)
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

            sortVerticies(verticiesOnEdge,0,verticiesOnEdge.size()-1);
            
            //Sanity check
            for(int a = 0; a < (int)verticiesOnEdge.size()-1; a++)
            {
                if(!(verticiesOnEdge[a]->val[X] < verticiesOnEdge[a+1]->val[X]))
                {
                    cout << "ERROR: Value not less than" << endl;
                    cout << verticiesOnEdge[a]->val[X] << " " <<  verticiesOnEdge[a+1]->val[X] << endl;
                 
                    exit(EXIT_FAILURE);
                }
            }
            
            for(int j = 0; j < (int)verticiesOnEdge.size()-1; j++)
            {
                Edge *edge = new Edge(verticiesOnEdge[j],verticiesOnEdge[j+1]);
                edge->edgestate.set = x;
                edges.push_back(edge);
                verticiesOnEdge[j]->edges[verticiesOnEdge[j]->currentEdge] = edge;
                verticiesOnEdge[j+1]->edges[verticiesOnEdge[j+1]->currentEdge] = edge;
                verticiesOnEdge[j]->currentEdge++;
                verticiesOnEdge[j+1]->currentEdge++;
            }   
        }
    }

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

    /*if(right-left < 2)
        return;

    //swap
    int randtemp = Utils::randInt(left,right);
    Vertex *temp = verticies[left];
    verticies[left] = verticies[randtemp];
    verticies[randtemp] = temp;
    int j = 0, i =0;
    float t;
    t = verticies[left]->val[X]; i =left; j = right+1;
    while(1){
        do{
            i++;
        }while(i <=right && verticies[i]->val[X] < t);
        do{
            j--;
        }while(verticies[j]->val[X] > t);
        if(i > j)
            break;
        temp = verticies[i];
        verticies[i] = verticies[j];
        verticies[j] = temp;
    }
    temp = verticies[left];
    verticies[left] = verticies[j];
    verticies[j] = temp;
    sortVerticies(verticies,left, j-1);
    sortVerticies(verticies,j+1,right);*/
}