/**
    \class Program
    \brief Loads in a vertex and fragment shader, shows errors if any
*/

#ifndef _PROGRAM_H
#define _PROGRAM_H

#include <gl/glew.h>

class Program
{
public:
    ///Compiles links a vertex and fragment shader and then returns a program id.
    GLuint       createProgram(char *vertexFilename, char *fragmentFilename);
    ///Links a vertex and fragment shader that have been compiled
    int          linkProgram(GLuint vertexShaderID, GLuint fragmentShaderID);
    ///Compiles a shader when given the source and type (GL_VERTEX_SHADER or GL_FRAGMENT SHADER)
    int          compileShader(const char *source, GLenum shaderType);
    ///Read in a shader file
    const char*  readShaderFile(char *fileName);
    ///Determines if the executables can be executed given the current OpenGL state
    void         validateProgram(GLuint programID);
    ///Gets the link info log (good to see if there were any errors during link time)
    int          linkInfoLog(GLuint programID);
    //Gets the compile info log (good to see if there were any errors during compile time)
    int          compileInfoLog(GLuint shaderID);
    //Lists all the variables in the program and prints them
    void         listProgramVariableDetails(GLuint pid);

private:
    ///Makes output look better
    unsigned int calculateTabs(int size);
    ///Finds the type name of the type entered
    char*        getTypeName(GLenum t);
};

#endif // _PROGRAM_H


