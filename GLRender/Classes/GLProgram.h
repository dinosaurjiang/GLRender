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
#define ALPHA "a_alpha"
#define TEXTURE1 "a_texCoord"


// 这些是不固定的。
// but,是默认都有的
enum Uniform
{
    GL_objMatrix,
    GL_pMatrix,// project matrix
    GL_mvMatrix,// model view matrix
    GL_alpha,
    GL_texture1,
    N_Uniform
};


enum
{
    ATTRIB_VERTEX,
    ATTRIB_COLOR,
    ATTRIB_TEXCOORD,
    
    NUM_ATTRIBUTES
};



// let OpenGL Using program.
// but this func can cache previous use.
// and will update program if need.
void UsingProgram(GLuint program);




#define STATIC_PROGROM( NAME, VSH, FSH) \
static GLProgram * default ## NAME ## Program () \
{ \
    static GLProgram * NAME ## _instance = nullptr; \
    if ( NAME ## _instance == nullptr ) \
    { \
        NAME ## _instance = new GLProgram(); \
        if( NAME ## _instance->loadShadersByName(VSH,FSH) == false) \
        {\
            Exception("load failed.", "can not load program for OpenGL:"  VSH  "," FSH);\
        }\
    }\
    return NAME ## _instance;\
};




using namespace std;

class GLProgram : GLBase
{
    
public:
    
    DECLARE_CLASS(GLProgram);
    
    GLProgram() = default;
    ~GLProgram();
    
    /*  load GLSL with given name of files.
     */
    bool loadShadersByName(const char * vsh, const char * fsh);
    bool loadShadersByName(string & vsh, string & fsh);
    
    /*  load with GLSL srouces strings.
     */
    bool loadShaders(const char * vsh, const char * fsh);
    bool validateProgram(GLuint prog);
    
    
    // default programs.
    STATIC_PROGROM(ColorDraw,"ColorDraw.vsh","ColorDraw.fsh")
    
    STATIC_PROGROM(TextureDraw,"TextureDraw.vsh","TextureDraw.fsh")
    
    STATIC_PROGROM(TextureColorDraw,"TextColorMix.vsh","TextColorMix.fsh")
    
    STATIC_PROGROM(ParticleSystem,"ParticleSystem.vsh","ParticleSystem.fsh")
    
    
    
    GLuint programID();
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
