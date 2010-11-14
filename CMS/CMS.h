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

    void display();
    void init();

    void continuousModelSynthesis(vector<Edge*> &edges, vector<Vertex*> &verticies);

    CMSModel input;
    Cuboid boundingbox;
    Cuboid world;
    Grid grid;
};