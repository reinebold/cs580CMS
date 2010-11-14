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

    void continuousModelSynthesis(Edge &edges, Vertex &verticies);

private:
    CMSModel input;
    Cuboid boundingbox;
    Cuboid world;
    Grid grid;
};