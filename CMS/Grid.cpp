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
#include <unordered_map>
#include <algorithm>


bool Grid::assignEdge(Vertex* begin, Vertex *end)
{
    unsigned long long x = (int)begin;
    unsigned long long y = (int)end;
    unsigned long long hash1 = ((x+y)*(x+y)+x-y)/2; //We need to combine two numbers that make an edge unique...
    unsigned long long hash2 = ((x+y)*(x+y)+y-x)/2; //Also do the opposite...because it would be the same edge...

    if(fastEdges.find( hash1 ) == fastEdges.end() && fastEdges.find( hash2 ) == fastEdges.end())
    {
        Edge *edge = new Edge(begin, end);
        begin->edges[begin->connectedEdges++] = edge;
        end->edges[end->connectedEdges++] = edge;
        edges.push_back(edge);
        fastEdges.insert(EdgeHash::value_type( hash1 , edge));
        fastEdges.insert(EdgeHash::value_type( hash2 , edge));
        return true;
    }
    else
    {
        return false;
    }
}

//from http://marknelson.us/attachments/next-permutation/NextCombination.tgz
template <class BidirectionalIterator>
bool Grid::next_combination(BidirectionalIterator first, BidirectionalIterator k,
                      BidirectionalIterator last) {
   if ((first == last) || (k == first) || (k == last)) return false;
   BidirectionalIterator i = first;
   BidirectionalIterator ii = last;
   
   ++i;
   if (i == last) return false;
   i = last;
   --i;
   
   i = k;
   --ii;
   while (i != first){ 
      if (*--i < *ii){ // Search down to find first comb entry less than final entry
         BidirectionalIterator j = k;
         while(!(*i < *j)) j++; // Find swap position [good-1-high|good-2-low]
         iter_swap(i,j); // Swap [good-2-high|good-1-low]
         i++;j++;ii=k; // Move to rotation positions
         rotate(i,j,last); // Rotate [good-2-low-high-good-1]
         while(j!=last){ ++j; ++ii;} // Find high good position
         rotate(k,ii,last); // Rotate [good-2-(low/high>|good-1-<low/high)]
         return true;
      }
   }
   rotate (first,k,last);
   return false;
}

Face *Grid::findFace(Vertex* v1, Vertex *v2, Vertex *v3, Vertex *v4)
{
    //if(v1 == NULL || v2 == NULL || v3 == NULL || v4 == NULL)
   // {
   //     return NULL;
   // }

    for(unsigned int g = 0; g < faces.size(); g++)
    {
        if((*(faces[g]->vertices[0]) == *v1 || *(faces[g]->vertices[0]) == *v2 ||
           *(faces[g]->vertices[0]) == *v3 || *(faces[g]->vertices[0]) == *v4) &&
           (*(faces[g]->vertices[1]) == *v1 || *(faces[g]->vertices[1]) == *v2 ||
           *(faces[g]->vertices[1]) == *v3 || *(faces[g]->vertices[1]) == *v4) &&
           (*(faces[g]->vertices[2]) == *v1 || *(faces[g]->vertices[2]) == *v2 ||
           *(faces[g]->vertices[2]) == *v3 || *(faces[g]->vertices[2]) == *v4) &&
           (*(faces[g]->vertices[3]) == *v1 || *(faces[g]->vertices[3]) == *v2 ||
           *(faces[g]->vertices[3]) == *v3 || *(faces[g]->vertices[3]) == *v4))
        {
            return faces[g];
        }
    }
    return NULL;
}

Vertex *Grid::findVertex(Vertex *v)
{
    if(v == NULL)
    {
        return NULL;
    }

    for(unsigned int g = 0; g < vertices.size(); g++)
    {
        if( fabs(v->val[X] - vertices[g]->val[X]) < 1.0f &&
            fabs(v->val[Y] - vertices[g]->val[Y]) < 1.0f &&
            fabs(v->val[Z] - vertices[g]->val[Z]) < 1.0f)
        {
            return vertices[g];
        }
    }
    return NULL;
}

bool Grid::edgeAlreadyInList(Edge *edge)
{
    if(edge == NULL)
    {
        return true;
    }
    bool same = false;
    for(unsigned int g = 0; g < edges.size(); g++)
    {
        if((*edge) == *(edges[g]))
        {
            same = true;
            break;
        }
    }
    if(same)
    {
        delete edge;
    }
    return same;
}

void Grid::init(const CMSModel3D &model, const Cuboid &boundingBox)
{
    /*
    TODO:
    1.  Need to define volumes and cubes.
    2.  Figure out how edges should be labeled.
    */

    Utils::Timer timer;
    cout << endl << "---Grid Progress---" << endl;

    //Go through each face of the model, and extract the DISTINCT normals (also they can't be the opposite of each other)
    cout << "Finding distinct normals...";
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
            float length = sqrt((model.faces[x].edges[y]->begin->val[X] - model.faces[x].edges[y]->end->val[X]) * (model.faces[x].edges[y]->begin->val[X] - model.faces[x].edges[y]->end->val[X]) +
                                (model.faces[x].edges[y]->begin->val[Y] - model.faces[x].edges[y]->end->val[Y]) * (model.faces[x].edges[y]->begin->val[Y] - model.faces[x].edges[y]->end->val[Y]));
            if(length > maxLength)
            {
                maxLength = length;
            }
        }
    }
    spacing = Utils::randFloat(maxLength/2.0f, maxLength);
    cout << spacing << endl;

    timer.start();
    cout << "Finding parallel faces..." << endl;
    for(unsigned int x = 0; x < distinctNormals.size(); x++)
    {
        Vector position;
        float increment = spacing; //Need to figure this out for planes that aren't parallel to the bounding box planes.
        position.x = distinctNormals[x].x;
        position.y = distinctNormals[x].y;
        position.z = distinctNormals[x].z;

        //Plane previous(Vertex(0.0f,0.0f,0.0f),Vector(0.0f,0.0f,0.0f));
        bool change = false;
        int currentPlane = 0;
        while(true)
        {
            Plane plane( Vertex(position.x+(currentPlane*increment*distinctNormals[x].x), position.y+(currentPlane*increment*distinctNormals[x].y), position.z+(currentPlane*increment*distinctNormals[x].z)), distinctNormals[x]);
            //float distance = Vector(plane.v.val[X] - previous.v.val[X],plane.v.val[Y] - previous.v.val[Y],plane.v.val[Z] - previous.v.val[Z]).magnitude();
        
            Edge* edges[6];
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
                if(change == false)
                {
                    change = true;
                    increment = -spacing;
                    currentPlane = 1;
                    continue;
                }
                else
                {
                    break;
                }
            }

            Face *face = createFace(edges, numEdges);
            face->set = x;
            parallelFaces[x].push_back(face);
            currentPlane++;
            //previous = plane;
        }
        cout << "\tGenerated planes for set " << x << "..." << parallelFaces[x].size() << endl;
    }
    timer.stop();
    timer.printSeconds();
    
    timer.start();
    cout << "Finding vertices of plane-plane-plane intersection...";
    //Have to do combinations if we have more than 3 distinct normals.
    int *numberArray = new int[parallelFaces.size()];
    for(int h = 0; h < (int)parallelFaces.size(); h++)  //Basically [0,1,2,3]
    {
        numberArray[h] = h;
    }
    do
    {
        for(unsigned int x = 0; x < parallelFaces[numberArray[0]].size(); x++)
        {
            for(unsigned int y = 0; y < parallelFaces[numberArray[1]].size(); y++)
            {
                for(unsigned int z = 0; z < parallelFaces[numberArray[2]].size(); z++)
                {
                    Vertex *vertex = faceFaceFaceIntersection(parallelFaces[numberArray[0]][x], parallelFaces[numberArray[1]][y], parallelFaces[numberArray[2]][z]);
                    
                    if(vertex != NULL && boundingBox.inside(vertex))
                    {
                        vertices.push_back(vertex);
                    }

                }
            }
        }
    } while(next_combination(numberArray,numberArray + 3,numberArray+(int)parallelFaces.size()));
    cout << vertices.size() << endl;
    timer.stop();
    timer.printSeconds();

    timer.start();
    cout << "Connecting vertices to edges...";
    do
    {
        for(unsigned int x = 0; x < parallelFaces[numberArray[0]].size(); x++)
        {
            for(unsigned int y = 0; y < parallelFaces[numberArray[1]].size(); y++)
            {
                Plane plane(*(parallelFaces[numberArray[0]][x]->vertices[0]),parallelFaces[numberArray[0]][x]->normal);
                Edge *edge = planeFaceIntersection(plane,*(parallelFaces[numberArray[1]][y]));

                

                if(edge != NULL)
                {
                    vector<Vertex *> verticesOnEdge;
                    for(unsigned int h = 0; h < vertices.size(); h++)
                    {
                        float length  = Vector(edge->begin->val[X] - edge->end->val[X], edge->begin->val[Y] - edge->end->val[Y], edge->begin->val[Z] - edge->end->val[Z]).magnitude();
                        float length1 = Vector(vertices[h]->val[X] - edge->end->val[X], vertices[h]->val[Y] - edge->end->val[Y], vertices[h]->val[Z] - edge->end->val[Z]).magnitude();
                        float length2 = Vector(edge->begin->val[X] - vertices[h]->val[X] , edge->begin->val[Y] -vertices[h]->val[Y] , edge->begin->val[Z] -vertices[h]->val[Z] ).magnitude();
                        if(fabs(length - (length1+length2)) < EPSILON)
                        {
                            verticesOnEdge.push_back(vertices[h]);
                        }
                    }

                    

                    if(verticesOnEdge.size() > 1)
                    {
                        //sortVertices3D(verticesOnEdge, 0, verticesOnEdge.size()-1,boundingBox);

                        for(unsigned int j = 0; j < verticesOnEdge.size()-1; j++)
                        {
                            Edge *edge = new Edge(verticesOnEdge[j],verticesOnEdge[j+1]);
                            edge->edgestate.set = x;
                            edge->connectedFaces = 0;
                            edges.push_back(edge);

                            verticesOnEdge[j]->edges[verticesOnEdge[j]->connectedEdges] = edge;
                            verticesOnEdge[j+1]->edges[verticesOnEdge[j+1]->connectedEdges] = edge;
                            verticesOnEdge[j]->connectedEdges++;
                            verticesOnEdge[j+1]->connectedEdges++;
                        } 
                    }
                }
            }
        }
    } while(next_combination(numberArray,numberArray + 2,numberArray+(int)parallelFaces.size()));
    cout << edges.size() << endl;
    timer.stop();
    timer.printSeconds();

    //THIS ONLY WORKS FOR CUBES RIGHT NOW
    if(distinctNormals.size() == 3)
    {
        timer.start();
        cout << "Connecting edges to faces...";
        for(unsigned int x = 0; x < parallelFaces[0].size(); x++)
        {
            for(unsigned int y = 0; y < parallelFaces[2].size() - 1; y++)
            {
                for(unsigned int z = 0; z < parallelFaces[1].size() - 1; z++)
                {
                    Plane plane(*(parallelFaces[0][x]->vertices[0]),parallelFaces[0][x]->normal);
                    Edge *edge1 = planeFaceIntersection(plane,*(parallelFaces[2][y]));
                    Edge *edge2 = planeFaceIntersection(plane,*(parallelFaces[2][y+1]));
                    Edge *edge3 = planeFaceIntersection(plane,*(parallelFaces[1][z]));
                    Edge *edge4 = planeFaceIntersection(plane,*(parallelFaces[1][z+1]));

                    if(edge1 == NULL || edge2 == NULL || edge3 == NULL || edge4 == NULL)
                    {
                        continue;
                    }
                    Vertex *vert1 = NULL;
                    Vertex *vert2 = NULL;
                    Vertex *vert3 = NULL;
                    Vertex *vert4 = NULL;
                    
                    if(edge1->intersect(*edge3, vert1) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge1->intersect(*edge4, vert2) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge2->intersect(*edge3, vert3) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge2->intersect(*edge4, vert4) != INTERESECTING)
                    {
                        continue;
                    }
                    Face *face = new Face();
                    face->vertices = new Vertex*[4];
                    if((face->vertices[0] = findVertex(vert1)) == NULL)
                    {
                        continue;
                    }
                    if((face->vertices[1] = findVertex(vert2)) == NULL)
                    {
                        continue;
                    }
                    if((face->vertices[2] = findVertex(vert3)) == NULL)
                    {
                        continue;
                    }
                    if((face->vertices[3] = findVertex(vert4)) == NULL)
                    {
                        continue;
                    }   


                    face->numVertices = 4;
                    face->numEdges = 0;

                    face->edges = new Edge*[4];
                    //Find edges and assign the edges to the face and faces to the edge
                    for(int u = 0; u < face->numVertices && face->numEdges != 4; u++)
                    {
                    for(int v = 0; v < face->vertices[u]->connectedEdges && face->numEdges != 4; v++)
                    {
                    Vertex *vert;
                    if(face->vertices[u]->edges[v]->begin == face->vertices[u])
                    {
                    vert = face->vertices[u]->edges[v]->end;
                    }
                    else if(face->vertices[u]->edges[v]->end == face->vertices[u])
                    {
                    vert = face->vertices[u]->edges[v]->begin;
                    }
                    else
                    {
                    cout << "I should never get here." << endl;
                    }

                    for(int w = 0; w < face->numVertices; w++)
                    {
                    if(u == w)
                    {
                    continue;
                    }
                    if(face->vertices[w] == vert)
                    {
                    bool same = false;
                    for(int z = 0; z < face->numEdges; z++)
                    {
                    if(face->vertices[u]->edges[v] == face->edges[z])
                    {
                    same = true;
                    break;
                    }
                    }
                    if(same == false)
                    {
                    face->edges[face->numEdges] = face->vertices[u]->edges[v];
                    face->vertices[u]->edges[v]->faces[face->vertices[u]->edges[v]->connectedFaces] = face;
                    face->vertices[u]->edges[v]->connectedFaces++;
                    face->numEdges++;
                    if(face->numEdges == 4)
                    {
                    break;
                    }
                    }
                    }
                    }
                    }
                    }
                    face->findNormal();
                    face->sortVertices();
                    for(unsigned int setNum = 0; setNum < distinctNormals.size(); setNum++)
                    {
                    if((distinctNormals[setNum] == face->normal ) || ( (fabs(distinctNormals[setNum].x - -face->normal.x) < EPSILON) &&
                    (fabs(distinctNormals[setNum].y - -face->normal.y) < EPSILON) &&
                    (fabs(distinctNormals[setNum].z - -face->normal.z) < EPSILON) ))
                    {
                    face->set = setNum;
                    break;
                    }
                    }
                    faces.push_back(face);
                }
            }
        }

        for(unsigned int x = 0; x < parallelFaces[1].size(); x++)
        {
            for(unsigned int y = 0; y < parallelFaces[0].size() - 1; y++)
            {
                for(unsigned int z = 0; z < parallelFaces[2].size() - 1; z++)
                {
                    Plane plane(*(parallelFaces[1][x]->vertices[0]),parallelFaces[1][x]->normal);
                    Edge *edge1 = planeFaceIntersection(plane,*(parallelFaces[0][y]));
                    Edge *edge2 = planeFaceIntersection(plane,*(parallelFaces[0][y+1]));
                    Edge *edge3 = planeFaceIntersection(plane,*(parallelFaces[2][z]));
                    Edge *edge4 = planeFaceIntersection(plane,*(parallelFaces[2][z+1]));

                    if(edge1 == NULL || edge2 == NULL || edge3 == NULL || edge4 == NULL)
                    {
                        continue;
                    }
                    Vertex *vert1 = NULL;
                    Vertex *vert2 = NULL;
                    Vertex *vert3 = NULL;
                    Vertex *vert4 = NULL;
                    
                    if(edge1->intersect(*edge3, vert1,1,2,0) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge1->intersect(*edge4, vert2,1,2,0) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge2->intersect(*edge3, vert3,1,2,0) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge2->intersect(*edge4, vert4,1,2,0) != INTERESECTING)
                    {
                        continue;
                    }
                    Face *face = new Face();

                    face->vertices = new Vertex*[4];
                    if((face->vertices[0] = findVertex(vert1)) == NULL)
                    {
                        continue;
                    }
                    if((face->vertices[1] = findVertex(vert2)) == NULL)
                    {
                        continue;
                    }
                    if((face->vertices[2] = findVertex(vert3)) == NULL)
                    {
                        continue;
                    }
                    if((face->vertices[3] = findVertex(vert4)) == NULL)
                    {
                        continue;
                    }   

                    face->numVertices = 4;
                    face->numEdges = 0;

                    face->edges = new Edge*[4];
                    //Find edges and assign the edges to the face and faces to the edge
                    for(int u = 0; u < face->numVertices && face->numEdges != 4; u++)
                    {
                        for(int v = 0; v < face->vertices[u]->connectedEdges && face->numEdges != 4; v++)
                        {
                            Vertex *vert;
                            if(face->vertices[u]->edges[v]->begin == face->vertices[u])
                            {
                                vert = face->vertices[u]->edges[v]->end;
                            }
                            else if(face->vertices[u]->edges[v]->end == face->vertices[u])
                            {
                                vert = face->vertices[u]->edges[v]->begin;
                            }
                            else
                            {
                                cout << "I should never get here." << endl;
                            }

                            for(int w = 0; w < face->numVertices; w++)
                            {
                                if(u == w)
                                {
                                    continue;
                                }
                                if(face->vertices[w] == vert)
                                {
                                    bool same = false;
                                    for(int z = 0; z < face->numEdges; z++)
                                    {
                                        if(face->vertices[u]->edges[v] == face->edges[z])
                                        {
                                            same = true;
                                            break;
                                        }
                                    }
                                    if(same == false)
                                    {
                                        face->edges[face->numEdges] = face->vertices[u]->edges[v];
                                        face->vertices[u]->edges[v]->faces[face->vertices[u]->edges[v]->connectedFaces] = face;
                                        face->vertices[u]->edges[v]->connectedFaces++;
                                        face->numEdges++;
                                        if(face->numEdges == 4)
                                        {
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    face->findNormal();
                    face->sortVertices();
                    for(unsigned int setNum = 0; setNum < distinctNormals.size(); setNum++)
                    {
                        if((distinctNormals[setNum] == face->normal ) || ( (fabs(distinctNormals[setNum].x - -face->normal.x) < EPSILON) &&
                                                                           (fabs(distinctNormals[setNum].y - -face->normal.y) < EPSILON) &&
                                                                           (fabs(distinctNormals[setNum].z - -face->normal.z) < EPSILON) ))
                        {
                            face->set = setNum;
                            break;
                        }
                    }
                    faces.push_back(face);
                }
            }
        }

        for(unsigned int x = 0; x < parallelFaces[2].size(); x++)
        {
            for(unsigned int y = 0; y < parallelFaces[0].size() - 1; y++)
            {
                for(unsigned int z = 0; z < parallelFaces[1].size() - 1; z++)
                {
                    Plane plane(*(parallelFaces[2][x]->vertices[0]),parallelFaces[2][x]->normal);
                    Edge *edge1 = planeFaceIntersection(plane,*(parallelFaces[0][y]));
                    Edge *edge2 = planeFaceIntersection(plane,*(parallelFaces[0][y+1]));
                    Edge *edge3 = planeFaceIntersection(plane,*(parallelFaces[1][z]));
                    Edge *edge4 = planeFaceIntersection(plane,*(parallelFaces[1][z+1]));

                    if(edge1 == NULL || edge2 == NULL || edge3 == NULL || edge4 == NULL)
                    {
                        continue;
                    }
                    Vertex *vert1 = NULL;
                    Vertex *vert2 = NULL;
                    Vertex *vert3 = NULL;
                    Vertex *vert4 = NULL;
                    
                    if(edge1->intersect(*edge3, vert1,0,2,1) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge1->intersect(*edge4, vert2,0,2,1) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge2->intersect(*edge3, vert3,0,2,1) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge2->intersect(*edge4, vert4,0,2,1) != INTERESECTING)
                    {
                        continue;
                    }
                    Face *face = new Face();

                    face->vertices = new Vertex*[4];
                    if((face->vertices[0] = findVertex(vert1)) == NULL)
                    {
                        continue;
                    }
                    if((face->vertices[1] = findVertex(vert2)) == NULL)
                    {
                        continue;
                    }
                    if((face->vertices[2] = findVertex(vert3)) == NULL)
                    {
                        continue;
                    }
                    if((face->vertices[3] = findVertex(vert4)) == NULL)
                    {
                        continue;
                    }   

                    face->numVertices = 4;
                    face->numEdges = 0;

                    face->edges = new Edge*[4];
                    //Find edges and assign the edges to the face and faces to the edge
                    for(int u = 0; u < face->numVertices && face->numEdges != 4; u++)
                    {
                        for(int v = 0; v < face->vertices[u]->connectedEdges && face->numEdges != 4; v++)
                        {
                            Vertex *vert;
                            if(face->vertices[u]->edges[v]->begin == face->vertices[u])
                            {
                                vert = face->vertices[u]->edges[v]->end;
                            }
                            else if(face->vertices[u]->edges[v]->end == face->vertices[u])
                            {
                                vert = face->vertices[u]->edges[v]->begin;
                            }
                            else
                            {
                                cout << "I should never get here." << endl;
                            }

                            for(int w = 0; w < face->numVertices; w++)
                            {
                                if(u == w)
                                {
                                    continue;
                                }
                                if(face->vertices[w] == vert)
                                {
                                    bool same = false;
                                    for(int z = 0; z < face->numEdges; z++)
                                    {
                                        if(face->vertices[u]->edges[v] == face->edges[z])
                                        {
                                            same = true;
                                            break;
                                        }
                                    }
                                    if(same == false)
                                    {
                                        face->edges[face->numEdges] = face->vertices[u]->edges[v];
                                        face->vertices[u]->edges[v]->faces[face->vertices[u]->edges[v]->connectedFaces] = face;
                                        face->vertices[u]->edges[v]->connectedFaces++;
                                        face->numEdges++;
                                        if(face->numEdges == 4)
                                        {
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    face->findNormal();
                    face->sortVertices();
                    for(unsigned int setNum = 0; setNum < distinctNormals.size(); setNum++)
                    {
                        if((distinctNormals[setNum] == face->normal ) || ( (fabs(distinctNormals[setNum].x - -face->normal.x) < EPSILON) &&
                            (fabs(distinctNormals[setNum].y - -face->normal.y) < EPSILON) &&
                            (fabs(distinctNormals[setNum].z - -face->normal.z) < EPSILON) ))
                        {
                            face->set = setNum;
                            break;
                        }
                    }
                    faces.push_back(face);
                }
            }
        }
        cout << faces.size() << endl;
        timer.stop();
        timer.printSeconds();

        cout << "Connecting faces to volumes...";
        timer.start();
        for(unsigned int x = 0; x < parallelFaces[0].size() - 1; x++)
        {
            for(unsigned int y = 0; y < parallelFaces[1].size() - 1; y++)
            {
                for(unsigned int z = 0; z < parallelFaces[2].size() - 1; z++)
                {
                    Plane plane1(*(parallelFaces[0][x]->vertices[0]),parallelFaces[0][x]->normal);
                    Edge *edge1  = planeFaceIntersection(plane1,*(parallelFaces[1][y]));
                    Edge *edge2  = planeFaceIntersection(plane1,*(parallelFaces[1][y+1]));
                    Edge *edge3  = planeFaceIntersection(plane1,*(parallelFaces[2][z]));
                    Edge *edge4  = planeFaceIntersection(plane1,*(parallelFaces[2][z+1]));

                    Plane plane2(*(parallelFaces[0][x+1]->vertices[0]),parallelFaces[0][x+1]->normal);
                    Edge *edge5  = planeFaceIntersection(plane2,*(parallelFaces[1][y]));
                    Edge *edge6  = planeFaceIntersection(plane2,*(parallelFaces[1][y+1]));
                    Edge *edge7  = planeFaceIntersection(plane2,*(parallelFaces[2][z]));
                    Edge *edge8  = planeFaceIntersection(plane2,*(parallelFaces[2][z+1]));

                    if(edge1 == NULL || edge2 == NULL || edge3 == NULL || edge4 == NULL ||
                       edge5 == NULL || edge6 == NULL || edge7 == NULL || edge8 == NULL)
                    {
                        continue;
                    }
                    Vertex *vert1 = NULL;
                    Vertex *vert2 = NULL;
                    Vertex *vert3 = NULL;
                    Vertex *vert4 = NULL;
                    Vertex *vert5 = NULL;
                    Vertex *vert6 = NULL;
                    Vertex *vert7 = NULL;
                    Vertex *vert8 = NULL;
                    
                    if(edge1->intersect(*edge3, vert1) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge1->intersect(*edge4, vert2) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge2->intersect(*edge3, vert3) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge2->intersect(*edge4, vert4) != INTERESECTING)
                    {
                        continue;
                    }
                    
                    if(edge5->intersect(*edge7, vert5) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge5->intersect(*edge8, vert6) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge6->intersect(*edge7, vert7) != INTERESECTING) 
                    {
                        continue;
                    }
                    if(edge6->intersect(*edge8, vert8) != INTERESECTING)
                    {
                        continue;
                    }

                    Face* face1 = NULL;
                    Face* face2 = NULL;
                    Face* face3 = NULL;
                    Face* face4 = NULL;
                    Face* face5 = NULL;
                    Face* face6 = NULL;

                    if((face1 = findFace(vert1,vert2,vert3,vert4)) == NULL)
                    {
                        continue;
                    }
                    if((face2 = findFace(vert5,vert6,vert7,vert8)) == NULL)
                    {
                        continue;
                    }
                    if((face3 = findFace(vert1,vert2,vert5,vert6)) == NULL)
                    {
                        continue;
                    }
                    if((face4 = findFace(vert3,vert4,vert7,vert8)) == NULL)
                    {
                        continue;
                    }
                    if((face5 = findFace(vert2,vert4,vert6,vert8)) == NULL)
                    {
                        continue;
                    }
                    if((face6 = findFace(vert1,vert3,vert5,vert7)) == NULL)
                    {
                        continue;
                    }

                    Volume *volume = new Volume();
                    volume->numFaces = 6;
                    volume->faces = new Face*[volume->numFaces];
                    volume->faces[0] = face1;
                    (face1->leftVolume == NULL) ? (face1->leftVolume = volume) : (face1->rightVolume = volume);
                    volume->faces[1] = face2;
                    (face2->leftVolume == NULL) ? (face2->leftVolume = volume) : (face2->rightVolume = volume);
                    volume->faces[2] = face3;
                    (face3->leftVolume == NULL) ? (face3->leftVolume = volume) : (face3->rightVolume = volume);
                    volume->faces[3] = face4;
                    (face4->leftVolume == NULL) ? (face4->leftVolume = volume) : (face4->rightVolume = volume);
                    volume->faces[4] = face5;
                    (face5->leftVolume == NULL) ? (face5->leftVolume = volume) : (face5->rightVolume = volume);
                    volume->faces[5] = face6;
                    (face6->leftVolume == NULL) ? (face6->leftVolume = volume) : (face6->rightVolume = volume);
                    volume->state = UNASSIGNED;
                    volumes.push_back(volume);
                }
            }
        }
        cout << volumes.size() << endl;
        timer.stop();
        timer.printSeconds();

    }
    else
    {
        cout << "Warning: Faces and Volumes not found." << endl;
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

void Grid::sortFaces(vector<Face*> &faces, int left, int right)
{
    if(left>=right)
        return;
    int middle = left;
    for(int i = left+1; i <= right; i++)
    {
        //Distance between parallel planes
        float d1 = faces[i]->normal.x * -faces[i]->vertices[0]->val[X] + faces[i]->normal.y * -faces[i]->vertices[0]->val[Y] + faces[i]->normal.z * -faces[i]->vertices[0]->val[Z];
        float d2 = faces[left]->normal.x * -faces[left]->vertices[0]->val[X] + faces[left]->normal.y * -faces[left]->vertices[0]->val[Y] + faces[left]->normal.z * -faces[left]->vertices[0]->val[Z];
        float distance1 = fabs(d1)/sqrt(faces[i]->normal.x*faces[i]->normal.x + faces[i]->normal.y*faces[i]->normal.y + faces[i]->normal.z*faces[i]->normal.z);
        float distance2 = fabs(d2)/sqrt(faces[left]->normal.x*faces[left]->normal.x + faces[left]->normal.y*faces[left]->normal.y + faces[left]->normal.z*faces[left]->normal.z);
        
        if(distance1 < distance2)
        {
            Face *temp = faces[++middle];
            faces[middle] = faces[i];
            faces[i] = temp;  
        }
    }
    Face *temp = faces[left];
    faces[left] = faces[middle];
    faces[middle] = temp;
    sortFaces(faces,left,middle-1);
    sortFaces(faces,middle+1,right);

}

void Grid::sortVertices3D(vector<Vertex*> &vertices, int left, int right, const Cuboid &bb)
{
    if(left>=right)
        return;
    int middle = left;
    for(int i = left+1; i <= right; i++)
    {
        float length1 = Vector(vertices[i]->val[X] - bb.minx, vertices[i]->val[Y] - bb.miny, vertices[i]->val[Z] - bb.minz).magnitude();
        float length2 = Vector(vertices[left]->val[X] - bb.minx, vertices[left]->val[Y] - bb.miny, vertices[left]->val[Z] - bb.minz).magnitude();
        if(length1 < length2)
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