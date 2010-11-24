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

void Grid::init(const CMSModel3D &model, const Cuboid &boundingBox)
{
    /*
    TODO:
    1.  Only works with a cube input.
    2.  Figure out how to get equidistant spacing between planes that are not parallel with the bounding box planes.
    3.  
    */

    cout << endl << "---Grid Progress---" << endl;

    //Go through each face of the model, and extract the DISTINCT normals (also they can't be the opposite of each other)
    cout << "Finding distinct edges...";
    vector<Vector> distinctNormals;
    for(int x = 0; x < model.numFaces; x++)
    {
        //Extract the normal from the face.
        Vector normal = model.faces[x].normal;
        normal.normalize();

        //Check to make sure it is distinct and not the opposite of the normal
        bool distinct = true;
        for(vector<Vector>::iterator i = distinctNormals.begin(); i < distinctNormals.end(); i++)
        {
            if(((*i) == normal) || ( (fabs((*i).x - -normal.x) < EPSILON) &&
                                     (fabs((*i).y - -normal.y) < EPSILON) &&
                                     (fabs((*i).z - -normal.z) < EPSILON) ))
            {
                distinct = false;
                break;
            }
        }

        if(distinct)
        {
            distinctNormals.push_back(normal);
            parallelFaces.push_back(vector<Face*>());
        }  
    }
    cout << distinctNormals.size() << endl;

    //Find a good spacing value by finding the maximum length of all edges in the model.  Then take a value between
    //[maxLength/2.0, maxLength]
    cout << "Finding a good spacing value...";
    float maxLength = 0.0;
    for(int x = 0; x < model.numFaces; x++)
    {
        for(int y = 0; y < model.faces[x].numEdges; y++)
        {
            float length = sqrt((model.faces[x].edges->begin->val[X] - model.faces[x].edges->end->val[X]) * (model.faces[x].edges->begin->val[X] - model.faces[x].edges->end->val[X]) +
                                (model.faces[x].edges->begin->val[Y] - model.faces[x].edges->end->val[Y]) * (model.faces[x].edges->begin->val[Y] - model.faces[x].edges->end->val[Y]));
            if(length > maxLength)
            {
                maxLength = length;
            }
        }
    }
    spacing = Utils::randFloat(maxLength/2.0f, maxLength);
    cout << spacing << endl;

    float increment = spacing; //Need to figure this out for planes that aren't parallel to the bounding box planes.
    Vector position(0.0f, 0.0f, 0.0f);
    for(unsigned int x = 0; x < distinctNormals.size(); x++)
    {
        cout << "Finding parallel planes of set " << x << "..." << endl;
        float currentPosition = 0.0f;
        float endPosition = 0.0f;
        if( (fabs(distinctNormals[x].x) > EPSILON) && (fabs(distinctNormals[x].y) < EPSILON) && (fabs(distinctNormals[x].z) < EPSILON) )
        {
            cout << "\tPlane parallel to x plane." << endl;
            position.x = 1.0f;
            endPosition = boundingBox.width;
        }
        else if( (fabs(distinctNormals[x].x) < EPSILON) && (fabs(distinctNormals[x].y) > EPSILON) && (fabs(distinctNormals[x].z) < EPSILON) )
        {
            cout << "\tPlane parallel to y plane." << endl;
            position.y = 1.0f;
            endPosition = boundingBox.height;
        }
        else if( (fabs(distinctNormals[x].x) < EPSILON) && (fabs(distinctNormals[x].y) < EPSILON) && (fabs(distinctNormals[x].z) > EPSILON) )
        {
            cout << "\tPlane parallel to z plane." << endl;
            position.z = 1.0f;
            endPosition = boundingBox.depth;
        }

        while(currentPosition < endPosition)
        {
            Plane plane(Vertex(position.x*currentPosition, position.y*currentPosition, position.z*currentPosition), distinctNormals[x]);
            Edge* edges[4];
            int  numEdges = 0; 
            for(int y = 0; y < boundingBox.numFaces; y++)
            {
                Edge *edge = planeFaceIntersection(plane, boundingBox.faces[y]);
                if(edge != NULL)
                {
                    edges[numEdges++] = edge;
                }
            }

            if(numEdges == 0)
            {
                currentPosition += increment;
                break;
            }

#ifdef _DEBUG
            //Sanity Check: Make sure the face has four edges.
            if(numEdges != 4)
            {
                cout << endl << "Error: A face did not contain 4 edges." << endl;
                exit(EXIT_FAILURE);
            }
#endif
            Face *face = createFace(edges, numEdges);
            face->set = x;
            parallelFaces[x].push_back(face);
            

            currentPosition += increment;
        }
        cout << "\tGenerated planes..." << parallelFaces[x].size() << endl;
    }

    cout << "Finding vertices of plane-plane-plane intersection...";
    for(unsigned int x = 0; x < parallelFaces[0].size(); x++)
    {
        for(unsigned int y = 0; y < parallelFaces[1].size(); y++)
        {
            for(unsigned int z = 0; z < parallelFaces[2].size(); z++)
            {
                Vertex *vertex = faceFaceFaceIntersection(parallelFaces[0][x], parallelFaces[1][y], parallelFaces[2][z]);
                vertices.push_back(vertex);
            }
        }
    }
    cout << vertices.size() << endl;

    /*
    cout << "Connecting vertices...";
    for(unsigned int x = 0; x < vertices.size(); x++)
    {
        int numSphereVertices = 0;
        Vertex* sphereVertices[7];
        for(unsigned int y = 0; y < vertices.size(); y++)
        {
            if(x == y)
            {
                continue;
            }

            if(vertexInSphere(vectices[x], vertices[y]))
            {
                sphereVertices[numSphereVertices++] = vertices[y];
            }

#ifdef _DEBUG
            //Sanity Check: Make sure the vertices aren't connected to more than 6 vertices.
            if(numSphereVertices > 6)
            {
                cout << "Error: Vertex is connected to more than 6 vertices." << endl;
                exit(EXIT_FAILURE);
            }
#endif
        }
#ifdef _DEBUG
        //Sanity Check: Make sure the vertex is connected to at least three other vertices
        if(numSphereVertices < 3)
        {
            cout << "Error: Vertex is connected to less than 3 vertices." << endl;
        }
#endif
        //Connect the vertices with edges...in a particular order, always towards positive z, positive y, or positive x.
        for(int z = 0; z < numSphereVertices; z++)
        {   
            if(fabs(vertices[x]->val[X] - vertices[y]->val[X]) < EPSILON)
            {
                if(fabs(vertices[x]->val[Y] - vertices[y]->val[Y]) < EPSILON)
                {
                    if(vertices[x]->val[Z] > vertices[x]->val[Z])
                    {
                        Edge* edge = new Edge(vertices[x], vertices[y]);
                        vertices[y]->edges[vertices[y]->connectedEdges++] = edge;
                        vertices[x]->edges[vertices[x]->connectedEdges++] = edge;
                        edges.push_back(edge);
                    }
                    else
                    {
                        Edge* edge = new Edge(vertices[y], vertices[x]);
                        vertices[y]->edges[vertices[y]->connectedEdges++] = edge;
                        vertices[x]->edges[vertices[x]->connectedEdges++] = edge;
                        edges.push_back(edge);
                    }
                }
                else if(vertices[x]->val[Y] > vertices[y]->val[Y])
                {
                    Edge* edge = new Edge(vertices[x], vertices[y]);
                    vertices[y]->edges[vertices[y]->connectedEdges++] = edge;
                    vertices[x]->edges[vertices[x]->connectedEdges++] = edge;
                    edges.push_back(edge);
                }
                else
                {
                    Edge* edge = new Edge(vertices[y], vertices[x]);
                    vertices[y]->edges[vertices[y]->connectedEdges++] = edge;
                    vertices[x]->edges[vertices[x]->connectedEdges++] = edge;
                    edges.push_back(edge);
                }
                
            }
            else if(vertices[x]->val[X] > vertices[y]->val[X])
            {
                //current vertex end, connected vertex begin
                Edge* edge = new Edge(vertices[x], vertices[y]);
                vertices[y]->edges[vertices[y]->connectedEdges++] = edge;
                vertices[x]->edges[vertices[x]->connectedEdges++] = edge;
                edges.push_back(edge);
            }
            else
            {
                Edge* edge = new Edge(vertices[y], vertices[x]);
                vertices[y]->edges[vertices[y]->connectedEdges++] = edge;
                vertices[x]->edges[vertices[x]->connectedEdges++] = edge;
                edges.push_back(edge);
            }
        }
    }
    cout << edges.size() << endl;
*/
    //Find some brute force way to get faces..

    //Find some brute force way to get volumes...


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