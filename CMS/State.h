/**
    \class State
    \brief Keeps track of general program information, like if the user selected to have lights
           on, or if the user wanted the axis to be drawn, etc.
*/

#ifndef _STATE_H
#define _STATE_H

class State
{
public:
    State()
        :lighting(false),
         drawLights(true),
         printInfoOnScreen(true),
         drawAxis(true),
         wireFrame(false),
         test(1)
    {
    }

    void setLighting(bool flag)         { lighting = flag; }
    void setDrawLights(bool flag)       { drawLights = flag; }
    void setPrintInfoOnScreen(bool flag){ printInfoOnScreen = flag; }
    void setDrawAxis(bool flag)         { drawAxis = flag; }
    void setWireFrame(bool flag)        { wireFrame = flag; }
    void setTest(int flag)              { test = flag; }     

    bool getWireFrame()                 { return wireFrame; }
    bool getDrawAxis()                  { return drawAxis; }
    bool getLighting()                  { return lighting; }
    bool getDrawLights()                { return drawLights; }
    bool getPrintInfoOnScreen()         { return printInfoOnScreen; }
    int  getTest()                      { return test; }

private:
    bool lighting;          ///< True if the lighting is enabled.
    bool drawLights;        ///< True if the user wants to see where the lights are
    bool printInfoOnScreen; ///< True if the user wants to see the info printed in the top left
    bool drawAxis;          ///< True if the user wants the axis to be drawn
    bool wireFrame;         ///< True if the user wants wireframe mode
    int  test;              ///< Use for testing stuff
};

#endif // _STATE_H