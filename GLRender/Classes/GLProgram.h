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


#define keyObjectMatrix         "a_objMatrix"
#define keyProjectMatrix        "a_pMatrix"
#define keyMVMatrix             "a_mvMatrix"
#define keyAlphaValue           "a_alpha"
#define keyTexture1             "a_texCoord"


#define keyAttrVertex   "a_position"
#define keyAttrColor    "a_color"
#define keyAttrTexCoord "a_texCoord"


// let OpenGL Using program.
// but this func can cache previous use.
// and will update program if need.
void UsingProgram(GLuint program);


using namespace std;


enum class DefaultUniform
{
    GL_ObjMatrix,
    GL_ProjectMatrix,// project matrix
    GL_MVMatrix,// model view matrix
    GL_AlphaValue,
    GL_Texture1
};


class GLProgram : GLBase
{
    
public:
    
    DECLARE_CLASS(GLProgram);
    
    // 这些是不固定的。
    // but,是默认都有的
    enum Attribute
    {
        ATTRIB_VERTEX,
        ATTRIB_COLOR,
        ATTRIB_TEXCOORD
    };
    
    
    
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
    
    GLuint programID();
    
    GLint attributeForName(string name);
    GLint uniformForName(string name);
    GLint getUniform(DefaultUniform type);
    
    
    
    bool setupObjectMatrix(GLfloat * mat);
    bool setupModelViewMatrix(GLfloat * mat);
    bool setupProjectionMatrix(GLfloat * mat);
    
    bool setMatrix4fForUniform(DefaultUniform type, GLfloat * values);
    bool setFloat1ForUniform(DefaultUniform type, GLfloat value);
    
protected:
    
    virtual void bind_attrbutes();
    
private:
    
    void updateAttrbu_Uniform();
    bool compileShader(GLuint * shader, GLenum type ,const GLchar * source);
    bool linkProgram(GLuint prog);
    
    
    GLuint      _program = 0;
    GLint       _uniformCache[5];
    
    
    map<string, GLint> _attrbute;
    map<string, GLint> _uniform;
};


#define ProgramCreateFunc(_NAME_,VSH,FSH) \
static GL ## _NAME_ ## Program * getInstance() {\
    static GL ## _NAME_ ## Program * _NAME_ ## _Instance_ = nullptr; \
    if (_NAME_ ## _Instance_ == nullptr) { \
        _NAME_ ## _Instance_ = new GL ## _NAME_ ## Program(); \
        if( _NAME_ ## _Instance_->loadShadersByName(VSH,FSH) == false) { \
            Exception("load failed.", "can not load program for OpenGL:"  VSH  "," FSH);\
        }\
    }\
    return _NAME_ ## _Instance_; }

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

class GLColorDrawProgram : public GLProgram
{
public:
    ProgramCreateFunc(ColorDraw,"ColorDraw.vsh","ColorDraw.fsh");
protected:
    
    virtual void bind_attrbutes();
};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

class GLTextureDrawProgram : public GLProgram
{
public:
    ProgramCreateFunc(TextureDraw,"TextureDraw.vsh","TextureDraw.fsh")
protected:
    
    virtual void bind_attrbutes();
};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

class GLTextureColorDrawProgram : public GLProgram
{
public:
    ProgramCreateFunc(TextureColorDraw,"TextColorMix.vsh","TextColorMix.fsh")
protected:
    
    virtual void bind_attrbutes();
};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

class GLParticleSysProgram : public GLProgram
{
public:
    ProgramCreateFunc(ParticleSys,"ParticleSystem.vsh","ParticleSystem.fsh")
protected:
    
    virtual void bind_attrbutes();
};

#endif /* defined(__GLRender__GLProgram__) */
