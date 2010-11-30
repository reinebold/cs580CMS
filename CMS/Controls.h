/**
    \class  Controls
    \brief  Class that keeps track of movement callback functions and things such as
            if the left button is pressed.   
*/

#ifndef _CONTROLS_H
#define _CONTROLS_H

#include <GL/glew.h>

class Controls
{
public:
    ///Callback function for use with glutMouseFunc
    static void mousePressHandler(int button, int state, int x, int y);
    ///Callback function for use with glutMotionFunc
    static void mouseMotionHandler(int x, int y);
    ///Callback function for use with the glutKeyboardFunc
    static void keyboardHandler(unsigned char key, int x, int y);
	///Callback function for use with the glutSpecialFunc
	static void specialKeyHandler(int key, int x, int y);

    //Gets
    bool  getLeftButton() const  { return leftButton; }
    bool  getRightButton() const { return rightButton; }
    GLint getOldx() const        { return oldx; }
    GLint getOldy() const        { return oldy; }

    //Sets
    void setLeftButton(bool flag) { leftButton = flag; }
    void setRightButton(bool flag){ rightButton = flag; }
    void setOldx(GLint value)     { oldx = value; }
    void setOldy(GLint value)     { oldy = value; }

private:
    bool    leftButton;     ///< If left button is currently pressed, it is true
    bool    rightButton;    ///< If right button is currently pressed, it is true

    GLint   oldx;           ///< Keeps track of the previous position of the mouse cursor for x
    GLint   oldy;           ///< Keeps track of the previous position of the mouse cursor for y
};

#endif // _CONTROLS_H