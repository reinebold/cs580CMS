Installation:
1. Put everything in 'Libraries/bin' in your windows system folder.
2. Compile and Run.
3. You should see a red cube.

Getting started:
1. Once you see the 'red cube' press 't' to cycle through some examples.
2. The first red cube show lighting
3. The teal cube shows how to use a program that uses shaders to color everything teal
4. The textured cube shows how to use textures.

Getting started coding:
1. A class 'CMS' was made in CMS.h and CMS.cpp.  It has 'display' and 'init' which are called in the
   corresponding 'display' and 'init' functions in Main.cpp.  So basically all you need to change is
   this class.

Updating doxygen:
1. Download doxygen.
2. In command prompt in the cs580CMS folder, do "doxygen Doxyfile"

Controlling the program:
'i' - toggles information on the screen
'esc' - exits the program
'l' - toggles lighting
'a' - toggles axis
'r' - resets transformations
'w' - toggles wireframe mode
't' - cycles through some examples on using textures/programs

'Left mouse button' - rotate
'Right mouse button' - translate
'Left and Right mouse buttons' - scale

Third Party Libraries used:
1. freeglut (for GUI): http://freeglut.sourceforge.net/
2. GLEW (for extensions): http://glew.sourceforge.net/
3. DevIL (for loading textures): http://openil.sourceforge.net/
4. Lib3ds (for loading 3ds models): http://code.google.com/p/lib3ds/
