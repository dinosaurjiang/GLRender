//
//  GLProgram.h
//  GLRender
//
//  Created by dinosaur on 13-11-28.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef __GLRender__GLProgram__
#define __GLRender__GLProgram__

#include <iostream>
#include "Utilities.h"
#include "mat4.h"
#include "GLObject.h"
#include <map>
#include "DefaultShader.h"


#define OBJ_MATRIX "a_objMatrix"
#define PROJECT_MATRIX "a_pMatrix"
#define MV_MATRIX "a_mvMatrix"
#define PIX_MODE "a_mode"
#define ALPHA "a_alpha"
#define TEXTURE1 "a_texCoord"


// 这些是不固定的。
// but,是默认都有的
enum Uniform
{
    GL_objMatrix,
    GL_pMatrix,// project matrix
    GL_mvMatrix,// model view matrix
    GL_mode, // color ? texture ? both?
    GL_alpha,
    GL_texture1,
    N_Uniform
};


// 这三个是固定的，必须有的
enum
{
    ATTRIB_VERTEX,
    ATTRIB_COLOR,
    ATTRIB_TEXCOORD,
    
    NUM_ATTRIBUTES
};


void UsingProgram(GLuint program);


using namespace std;

class GLProgram : GLBase
{
    
public:
    
    DECLARE_CLASS(GLProgram);
    
    GLProgram() = default;
    ~GLProgram();
    
    bool loadShadersByName(const char * vsh, const char * fsh);
    bool loadShaders(string & vsh, string & fsh);
    bool loadShaders(const char * vsh, const char * fsh);
    bool validateProgram(GLuint prog);
    
    static GLProgram * defaultProgram()
    {
        static GLProgram * _instance = nullptr;
        if (_instance == nullptr)
        {
            _instance = new GLProgram();
            if(_instance->loadShaders(defaultVertexShader,defaultFregmentShader) == false)
            {
                Exception("load failed.", "can not load default program for OpenGL");
            }
        }
        return _instance;
    };
    
    GLuint programID()
    {
        return _program;
    }
    
    GLint attributeForName(string name);
    GLint uniformForName(string name);
    
private:
    
    void updateAttrbu_Uniform();
    bool compileShader(GLuint * shader, GLenum type ,const GLchar * source);
    bool linkProgram(GLuint prog);
    GLuint _program = 0;
    
    map<string, GLint> attrbute;
    map<string, GLint> uniform;
};


#endif /* defined(__GLRender__GLProgram__) */
