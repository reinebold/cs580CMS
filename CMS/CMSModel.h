/**
    \class CMSModel
    \brief Contains information for the model that was given via the text file.
*/

#ifndef _CMSMODEL_H
#define _CMSMODEL_H

#include "Geometry.h"
using namespace Geometry;

class CMSModel
{
public:
    CMSModel();     
    ~CMSModel();    ///< Deallocates verticies and edges

    ///Fills in the data members.
    void init(int _numVerticies, Vertex *_verticies);
   
    int         numVerticies;      ///< The number of verticies of the model
    Vertex    *verticies;         ///< The verticies of the model
    Edge      *edges;             ///< The edges of the model
};


#endif // _CMSMODEL_H