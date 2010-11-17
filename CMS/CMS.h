/**
    \class CMS
    \brief Class for continuous model synthesis
*/

#include <GL/glew.h>
#include "CMSModel.h"
#include "Geometry.h"
#include "Grid.h"

class CMS
{
public:
    CMS();

    void display2D();   ///< If input model only has one face, display 2D
    void display3D();   ///< If input model has more than one face, display 3D
    void init();        ///< Initializes everything - input, boundingbox, world, and grid.

    void continuousModelSynthesis(vector<Edge*> &edges, vector<Vertex*> &verticies);    ///< Does the actual CMS algorithm

    CMSModel input;         ///< The input model
    Cuboid   boundingBox;   ///< Bounding box
    Cuboid   world;         ///< The "world", bounding box must be inside this
    Grid     grid;          ///< Grid that splits up the bounding box into edges/vertices/planes etc
    bool     showBoundingBox; ///< True if you want the bounding box to show.  Default true.
};