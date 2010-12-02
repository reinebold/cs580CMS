/**
    \class CMS
    \brief Class for continuous model synthesis
*/

#include <GL/glew.h>
#include "CMSModel.h"
#include "CMSModel3D.h"
#include "Geometry.h"
#include "Grid.h"
#include "Parser.h"
#include "Texture.h"
//#include <IL/il.h>
//#include <IL/ilu.h>

class CMS
{
public:
    CMS();

    void display2D();   ///< If input model only has one face, display 2D
    void display3D();   ///< If input model has more than one face, display 3D
    void init();        ///< Initializes everything - input, boundingbox, world, and grid.

    void continuousModelSynthesis(vector<Edge*> &edges, vector<Vertex*> &vertices);    ///< Does the actual CMS algorithm
    void continuousModelSynthesis3D(vector<Edge*> &edges, vector<Vertex*> &vertices);

    CMSModel input;         ///< The input model for 2D
    CMSModel3D input3D;     ///< The input model for 3D
	CMSModel3D cube;
	Parser   parser;
    Cuboid   boundingBox;   ///< Bounding box
    Cuboid   world;         ///< The "world", bounding box must be inside this
    Grid     grid;          ///< Grid that splits up the bounding box into edges/vertices/planes etc

    Texture texCloudsSide;
    Texture texCloudsTop;
    Texture texGrass;
    Texture texBuilding;
    Texture texCement;
    Texture texStreet;

    Texture texSkyBox[6];
	//ILuint texids[3];
	//GLuint texture;

    bool     showBoundingBox;   ///< True if you want the bounding box to show.  Default true.
    bool     showGridVertices; ///< True if you want to see all the vertices.  Default true.
    bool     showGridEdges;
    bool     showGridVolumes;
    bool     showGridFaces;
    bool     showTexture;
    bool     recreateCity;
    bool     showModel;
};