/**
    \file   Program.cpp
    \brief
*/

#include "Program.h"
#include <iostream>
using namespace std;

const int NumTypes = 27;

GLenum diffTypes[] = 
{
    GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_INT, GL_INT_VEC2, GL_INT_VEC3, 
    GL_INT_VEC4, GL_BOOL, GL_BOOL_VEC2, GL_BOOL_VEC3, GL_BOOL_VEC4, GL_FLOAT_MAT2, GL_FLOAT_MAT3, 
    GL_FLOAT_MAT4, GL_FLOAT_MAT2x3, GL_FLOAT_MAT2x4, GL_FLOAT_MAT3x2, GL_FLOAT_MAT3x4, GL_FLOAT_MAT4x2, 
    GL_FLOAT_MAT4x3, GL_SAMPLER_1D, GL_SAMPLER_2D, GL_SAMPLER_3D, GL_SAMPLER_CUBE, GL_SAMPLER_1D_SHADOW, 
    GL_SAMPLER_2D_SHADOW
};

char *typeNames[] = 
{
    "GL_FLOAT", "GL_FLOAT_VEC2", "GL_FLOAT_VEC3", "GL_FLOAT_VEC4", "GL_INT", "GL_INT_VEC2", "GL_INT_VEC3", 
    "GL_INT_VEC4", "GL_BOOL", "GL_BOOL_VEC2", "GL_BOOL_VEC3", "GL_BOOL_VEC4", "GL_FLOAT_MAT2", "GL_FLOAT_MAT3", 
    "GL_FLOAT_MAT4", "GL_FLOAT_MAT2x3", "GL_FLOAT_MAT2x4", "GL_FLOAT_MAT3x2", "GL_FLOAT_MAT3x4", "GL_FLOAT_MAT4x2", 
    "GL_FLOAT_MAT4x3", "GL_SAMPLER_1D", "GL_SAMPLER_2D", "GL_SAMPLER_3D", "GL_SAMPLER_CUBE", "GL_SAMPLER_1D_SHADOW", 
    "GL_SAMPLER_2D_SHADOW"
};

char* Program::getTypeName(GLenum t)
{
    for (int i=0; i<NumTypes;i++) 
    {
        if (t==diffTypes[i]) 
        {
            return typeNames[i]; 
        }
    }
    return "Unknown type";
}

unsigned int Program::calculateTabs(int size)
{
    if(size < 10)
        return 3;
    else if(size < 16)
        return 2;
    else
        return 1;
}

void Program::listProgramVariableDetails(GLuint pid)
{
    int maxUniforms;
    int maxUlen;
    glGetProgramiv(pid, GL_ACTIVE_UNIFORMS, &maxUniforms);
    glGetProgramiv(pid, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUlen);

    cout << "Uniform names:\n";
    GLchar *ubuf=(char *)malloc(maxUlen+1);
    for (int i=0; i < maxUniforms; i++)
    {
        int size;
        GLenum type;
        glGetActiveUniform(pid,i,maxUlen,NULL,&size,&type,ubuf);
        cout << "\tName: " << ubuf;
        for(unsigned int x = 0; x < calculateTabs(strlen(ubuf)); x++)
        {
            cout << "\t";
        }
        cout << "Type: " << getTypeName(type) << "\tLength: " << size << endl;
    }
    free(ubuf);

    int maxAttributes;
    int maxAlen;
    glGetProgramiv(pid, GL_ACTIVE_ATTRIBUTES,&maxAttributes);
    glGetProgramiv(pid, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAlen);

    cout << "Attribute names:\n";
    GLchar *abuf=(char *)malloc(maxAlen+1);
    for (int i=0 ; i < maxAttributes; i++)
    {
        int size;
        GLenum type;
        glGetActiveAttrib(pid,i,maxAlen,NULL,&size,&type,abuf);
        cout << "\tName: " << abuf;
        for(unsigned int x = 0; x < calculateTabs(strlen(abuf)); x++)
        {
            cout << "\t";
        }
        cout << "Type: " << getTypeName(type) << "\tLength: " << size << endl;
    }
    free (abuf);
    cout << endl;
}

int Program::compileInfoLog(GLuint shaderID)
{
    GLint status;
    // Check for any compile time error.
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (status==GL_FALSE)  // Error in shader code compilation.
    { 
        GLint infologLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infologLength);
        if(infologLength == 0)
        {
            cerr << "ERROR: No documented compilation error.\n";
            glDeleteShader(shaderID);
            return 0;
        }

        char *infoLog = (char *)malloc(infologLength);
        if (infoLog == NULL)
        {
            cerr << "ERROR: Can not allocate space to read compile error log\n";
            glDeleteShader(shaderID);
            return 0;
        }

        int readChars  = 0;
        glGetShaderInfoLog(shaderID, infologLength, &readChars, infoLog);
        cerr << "ERROR: " << infoLog << endl; 
        free(infoLog);
        glDeleteShader(shaderID);
        return 0;
    }
    else
    {
        cout << "No compilation error." << endl;
    }
    return shaderID;
}

int Program::linkInfoLog(GLuint programID)
{
    GLint status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    if (status==GL_FALSE)   // Error in linking shaders.
    { 
        GLint infologLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infologLength);
        if (infologLength==0)
        {
            cerr << "ERROR: No documented linking error.\n";
            glDeleteProgram(programID);
            return 0;
        }

        char *infoLog = (char *)malloc(infologLength);
        if (infoLog == NULL)
        {
            cerr << "ERROR: Can not allocate space to read linking error log.\n";
            glDeleteProgram(programID);
            return 0;
        }

        int readChars  = 0;
        glGetProgramInfoLog(programID, infologLength, &readChars, infoLog);
        cerr << "ERROR: " << infoLog << endl;  
        free(infoLog);
        glDeleteProgram(programID);
        return 0;
    }
    else 
    {
        cout << "No linking error." << endl;
    }
    return programID;
}

void Program::validateProgram(GLuint programID)
{
    //glValidateProgram checks to see whether the executables contained in program can 
    //execute given the current OpenGL state. The information generated by the validation 
    //process will be stored in program's information log.
    glValidateProgram(programID);

    GLint status;
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &status);
    if (status==GL_FALSE)
    {
        GLint infologLength;
        glGetProgramiv(programID,GL_INFO_LOG_LENGTH,&infologLength);
        if (infologLength==0)
        {
            cerr << "ERROR: No documented validation message.\n";
            return;
        }
        char *infoLog = (char *)malloc(infologLength);
        if (infoLog == NULL)
        {
            cerr << "ERROR: Can not allocate space to read validation message log.\n";
            return;
        }
        int readChars = 0;
        glGetProgramInfoLog(programID, infologLength, &readChars, infoLog);
        cout << "ERROR: " << infoLog << endl;
        free(infoLog);
    }
    else
    {
        cout << "No validation error.\n";
    }
}

const char* Program::readShaderFile(char *fileName)
{
    FILE *file;
    if (fileName == NULL) 
    {
        cerr << "Null Shader Code file name.\n" << endl;
        return NULL;
    }
    file = fopen(fileName,"rb");
    if (file == NULL)
    {
        cerr << "Can not open shader file: " << fileName << endl;
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    int fileLength = ftell(file);
    if ( 0 == fileLength)
    {
        cerr << "Empty shader file: " << fileName << endl; 
        fclose(file); 
        return NULL;
    }
    rewind(file);
    char *code = (char *)malloc(sizeof(char) * (fileLength+1)); // Keep an extra space for space
    if (code == NULL)
    {
        cerr << "Can not allocate space to read the shader code from file: " << fileName << endl;
        fclose(file); 
        return NULL;
    }
    int charsRead = 0;
    if ((charsRead = fread(code,sizeof(char),fileLength,file)) != fileLength) // Read the whole code into a single string.
    { 
        cerr << "Error: Chars read = " << charsRead << ", Actual file size: " << fileLength << " in " << fileName << endl; 
        fclose(file); 
        return NULL;
    }
    fclose(file);
    code[fileLength] = '\0';
    return code;
}

int Program::compileShader(const char *source, GLenum shaderType) 
{
    (shaderType == GL_VERTEX_SHADER) ? cout << "Vertex shader:   \t" : cout << "Fragment shader:\t";

    //glCreateShader creates an empty shader object and returns a non-zero value by 
    //which it can be referenced. A shader object is used to maintain the source code 
    //strings that define a shader. 
    GLuint shaderID = glCreateShader(shaderType);
    if(0 == shaderID)
    {
        cerr << "ERROR: Cannot create shader" << endl;
        return 0; 
    }
    //glShaderSource sets the source code in shader to the source code in the array of strings specified by string. 
    glShaderSource(shaderID, 1, &source, NULL);
    // glCompileShader compiles the source code strings that have been stored in the shader object specified by shader.
    glCompileShader(shaderID);
    return compileInfoLog(shaderID);
}

int Program::linkProgram(GLuint vertexShaderID, GLuint fragmentShaderID)
{
    //glCreateProgram creates an empty program object and returns a non-zero value by 
    //which it can be referenced. A program object is an object to which shader objects 
    //can be attached.
    GLuint programID = glCreateProgram();

    // Shaders that are to be linked together in a program object must first be attached 
    // to that program object. glAttachShader attaches the shader object specified by 
    // the second argument to the program object specified by the first argument. 
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    cout << "Program (link):\t\t";
    glLinkProgram(programID);
    linkInfoLog(programID);

    cout << "Program (validate):\t";
    validateProgram(programID);

    return programID;
}

GLuint Program::createProgram(char *vertexFilename, char *fragmentFilename)
{
    cout << "Program Information" << endl;
    cout << "-------------------" << endl;

    const char* vertexSource   = readShaderFile(vertexFilename);
    const char* fragmentSource = readShaderFile(fragmentFilename); 

    GLuint vertexShaderID    = compileShader(vertexSource,   GL_VERTEX_SHADER);
    GLuint fragmentShaderID  = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    GLuint programID         = linkProgram(vertexShaderID, fragmentShaderID);

    listProgramVariableDetails(programID);

    return programID;
}