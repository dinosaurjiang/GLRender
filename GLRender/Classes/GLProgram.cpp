//
//  GLProgram.cpp
//  GLRender
//
//  Created by dinosaur on 13-11-28.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#include "GLProgram.h"
#include "GLRenderPlatformHelper.h"
#include "GLRenderDef.h"

void UsingProgram(GLuint program)
{
#if OPEN_GL_CACHE
    static GLuint _cache_program = -1;
    if (program == _cache_program)
        return;
    
    glUseProgram(program);
    
    _cache_program = program;
#else
    glUseProgram(program);
#endif
}


GLProgram::~GLProgram()
{
    glDeleteProgram(_program);
    this->_program = 0;
}

bool GLProgram::loadShadersByName(const char * vsh, const char * fsh)
{
    string tvsh(vsh);
    string tfsh(fsh);
    return this->loadShadersByName(tvsh, tfsh);
}

bool GLProgram::loadShadersByName(string & vsh, string & fsh)
{
    char * vertexBuff = NULL;
    char * fragmentBuff = NULL;
    
    long vlen,flen;
    bool ret = false;
    
    vlen = loadBundleFileContextWithNameAPPLE(vsh, &vertexBuff);
    if( vlen == 0 )
        goto FAILED;
    
    flen = loadBundleFileContextWithNameAPPLE(fsh, &fragmentBuff);
    if(flen == 0)
        goto FAILED;
    
    ret = this->loadShaders(vertexBuff, fragmentBuff);
    

FAILED:
    
    if(vertexBuff)
        free(vertexBuff);
    if(fragmentBuff)
        free(fragmentBuff);
    
    return ret;
}

bool GLProgram::loadShaders(const char * vsh, const char * fsh)
{
    GLuint vertShader = 0, fragShader = 0;
    
    // Create shader program.
    this->_program = glCreateProgram();
    
    // Create and compile vertex shader.
    if (!this->compileShader(&vertShader, GL_VERTEX_SHADER, vsh))
    {
        LOG("Failed to compile vertex shader");
        return false;
    }
    
    // Create and compile fragment shader.
    
    if (!(this->compileShader(&fragShader, GL_FRAGMENT_SHADER, fsh)))
    {
        LOG("Failed to compile fragment shader");
        return false;
    }
    
    // Attach vertex shader to program.
    glAttachShader(_program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(_program, fragShader);
    
    
    this->bind_attrbutes();
    
    
    // Link program.
    if (!(this->linkProgram(_program)))
    {
        LOG("Failed to link program:%d" ,_program);
        
        if (vertShader)
        {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader)
        {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (_program)
        {
            glDeleteProgram(_program);
            _program = 0;
        }
        
        return false;
    }
    
    this->updateAttrbu_Uniform();
    
    // Release vertex and fragment shaders.
    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);
    
    glActiveTexture(GL_TEXTURE0);
//    glUniform1f(this->attributeForName(TEXTURE1), 0);
    
    return true;
}


bool GLProgram::compileShader(GLuint * shader, GLenum type ,const GLchar * source)
{
    GLint status;
    
    if (!source)
    {
        LOG("Failed to load vertex shader");
        return false;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        LOG("Shader compile log:[%s]" , log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(*shader);
        return false;
    }
    
    return true;
}


bool GLProgram::linkProgram(GLuint prog)
{
    GLint status;
    
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        LOG("link program log:[%s]" , log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0)
        return false;
    
    return true;
}

bool GLProgram::validateProgram(GLuint prog)
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        LOG("Program validate log:[%s]" , log );
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0)
        return false;
    
    return true;
}

void GLProgram::updateAttrbu_Uniform()
{
    _attrbute.clear();
    _uniform.clear();
    
    LOG("=====>START:%s<=====",__func__);
    
    //Attributes
    const int MAX_NAME_LENGTH = 64;
    char rawName[MAX_NAME_LENGTH];
    
    int numAttributes = 0;
    glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numAttributes);
    
    for (int i=0; i<numAttributes; ++i)
    {
        glGetActiveAttrib(_program, i, MAX_NAME_LENGTH, NULL, NULL, NULL, rawName);
        string name(rawName);
        _attrbute[name] = glGetAttribLocation(_program, rawName);
        LOG("found attribute:%s" , name.c_str() );
    }
    
    // for uniforms
    int numUniforms = 0;
    glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numUniforms);
    for (int i=0; i<numUniforms; ++i)
    {
        glGetActiveUniform(_program, i, MAX_NAME_LENGTH, NULL, NULL, NULL, rawName);
        string name(rawName);
        _uniform[name] = glGetUniformLocation(_program, rawName);
        LOG("found uniforms:%s" , name.c_str() );
    }
    
    LOG("=====>END:%s<=====",__func__);
    
    
    ///// update uniform
    
    _uniformCache[static_cast<int>(DefaultUniform::GL_ObjMatrix)] = this->uniformForName(keyObjectMatrix);
    _uniformCache[static_cast<int>(DefaultUniform::GL_ProjectMatrix)] = this->uniformForName(keyProjectMatrix);
    _uniformCache[static_cast<int>(DefaultUniform::GL_MVMatrix)] = this->uniformForName(keyMVMatrix);
    _uniformCache[static_cast<int>(DefaultUniform::GL_AlphaValue)] = this->uniformForName(keyAlphaValue);
    _uniformCache[static_cast<int>(DefaultUniform::GL_Texture1)] = this->uniformForName(keyTexture1);
}


GLuint GLProgram::programID()
{
    return _program;
}

void GLProgram::bind_attrbutes()
{
    glBindAttribLocation(_program, GLProgram::ATTRIB_VERTEX,   keyAttrVertex);
    glBindAttribLocation(_program, GLProgram::ATTRIB_COLOR,    keyAttrColor);
    glBindAttribLocation(_program, GLProgram::ATTRIB_TEXCOORD, keyAttrTexCoord);
}


GLint GLProgram::attributeForName(std::string name)
{
    map<string, GLint>::iterator  it = _attrbute.find(name);
    if (it == _attrbute.end())
    {
        // not find
        LOG( "not found attribute by name:[%s]" , name.c_str() );
        return -1;
    }
    return it->second;
}

GLint GLProgram::getUniform(DefaultUniform type)
{
    return _uniformCache[static_cast<int>(type)];
}

GLint GLProgram::uniformForName(std::string name)
{
    map<string, GLint>::iterator  it = _uniform.find(name);
    if (it == _uniform.end())
    {
        // not find
        LOG( "not found uniform by name:[%s]" , name.c_str() );
        return -1;
    }
    return it->second;
}

bool GLProgram::setFloat1ForUniform(DefaultUniform type, GLfloat value)
{
    int index = static_cast<int>(type);
    if (_uniformCache[index] != -1)
    {
        glUniform1f(_uniformCache[index],  value);
        return true;
    }
    return false;
}

bool GLProgram::setMatrix4fForUniform(DefaultUniform type, GLfloat * values)
{
    int index = static_cast<int>(type);
    if (_uniformCache[index] != -1)
    {
        glUniformMatrix4fv(_uniformCache[index], 1, 0, values);
        return true;
    }
    return false;
}

bool GLProgram::setupObjectMatrix(GLfloat * mat)
{
    return this->setMatrix4fForUniform(DefaultUniform::GL_ObjMatrix, mat);
}

bool GLProgram::setupModelViewMatrix(GLfloat * mat)
{
    return this->setMatrix4fForUniform(DefaultUniform::GL_MVMatrix, mat);
}

bool GLProgram::setupProjectionMatrix(GLfloat * mat)
{
    return this->setMatrix4fForUniform(DefaultUniform::GL_ProjectMatrix, mat);
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void GLColorDrawProgram::bind_attrbutes()
{
    glBindAttribLocation(this->programID(), GLProgram::ATTRIB_VERTEX,   keyAttrVertex);
    glBindAttribLocation(this->programID(), GLProgram::ATTRIB_COLOR,    keyAttrColor);
}



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void GLTextureDrawProgram::bind_attrbutes()
{
    glBindAttribLocation(this->programID(), GLProgram::ATTRIB_VERTEX,   keyAttrVertex);
    glBindAttribLocation(this->programID(), GLProgram::ATTRIB_TEXCOORD, keyAttrTexCoord);
}



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void GLTextureColorDrawProgram::bind_attrbutes()
{
    glBindAttribLocation(this->programID(), GLProgram::ATTRIB_VERTEX,   keyAttrVertex);
    glBindAttribLocation(this->programID(), GLProgram::ATTRIB_COLOR,    keyAttrColor);
    glBindAttribLocation(this->programID(), GLProgram::ATTRIB_TEXCOORD, keyAttrTexCoord);
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void GLParticleSysProgram::bind_attrbutes()
{
    glBindAttribLocation(this->programID(), GLProgram::ATTRIB_VERTEX,   keyAttrVertex);
    glBindAttribLocation(this->programID(), GLProgram::ATTRIB_COLOR,    keyAttrColor);
    glBindAttribLocation(this->programID(), GLProgram::ATTRIB_TEXCOORD, keyAttrTexCoord);
}
