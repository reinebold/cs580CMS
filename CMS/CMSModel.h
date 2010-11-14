

#ifndef _CMSMODEL_H
#define _CMSMODEL_H

#include "Geometry.h"
using namespace Geometry;

class CMSModel
{
public:
    CMSModel() {}

    void init(int numVerticies, Vertex *verticies);
   
    ~CMSModel()
    {
        delete [] _verticies;
        delete [] _edges;
    }

    int         _numVerticies;
    Vertex     *_verticies;
    Edge       *_edges;
};


#endif // _CMSMODEL_H