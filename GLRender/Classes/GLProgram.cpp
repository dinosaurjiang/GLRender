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
    return this->loadShadersByName(vsh, fsh, (color_flag|texcoord_flag));
}

bool GLProgram::loadShadersByName(const char * vsh, const char * fsh, int flag)
{
    string tvsh(vsh);
    string tfsh(fsh);
    return this->loadShadersByName(tvsh, tfsh, flag);
}

bool GLProgram::loadShadersByName(string & vsh, string & fsh)
{
    return this->loadShadersByName(vsh, fsh, (color_flag|texcoord_flag));
}

bool GLProgram::loadShadersByName(string & vsh, string & fsh, int flag)
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
    
    ret = this->loadShaders(vertexBuff, fragmentBuff, flag);
    

FAILED:
    
    if(vertexBuff)
        free(vertexBuff);
    if(fragmentBuff)
        free(fragmentBuff);
    
    return ret;
}

bool GLProgram::loadShaders(const char * vsh, const char * fsh)
{
    return this->loadShaders(vsh,fsh,(color_flag|texcoord_flag));
}


bool GLProgram::loadShaders(const char * vsh, const char * fsh, int flag)
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
    
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    // position must be load
    glBindAttribLocation(_program, GLProgram::ATTRIB_VERTEX,   "a_position");
    
    if(flag & color_flag)
        glBindAttribLocation(_program, GLProgram::ATTRIB_COLOR,    "a_color");
    
    if(flag & texcoord_flag)
        glBindAttribLocation(_program, GLProgram::ATTRIB_TEXCOORD, "a_texCoord");
    
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
    attrbute.clear();
    uniform.clear();
    
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
        attrbute[name] = glGetAttribLocation(_program, rawName);
        LOG("found attribute:%s" , name.c_str() );
    }
    
    // for uniforms
    int numUniforms = 0;
    glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numUniforms);
    for (int i=0; i<numUniforms; ++i)
    {
        glGetActiveUniform(_program, i, MAX_NAME_LENGTH, NULL, NULL, NULL, rawName);
        string name(rawName);
        uniform[name] = glGetUniformLocation(_program, rawName);
        LOG("found uniforms:%s" , name.c_str() );
    }
    
    LOG("=====>END:%s<=====",__func__);
}


GLuint GLProgram::programID()
{
    return _program;
}


GLint GLProgram::attributeForName(std::string name)
{
    map<string, GLint>::iterator  it = attrbute.find(name);
    if (it == attrbute.end())
    {
        // not find
        LOG( "not found attribute by name:[%s]" , name.c_str() );
        return -1;
    }
    return it->second;
}

GLint GLProgram::uniformForName(std::string name)
{
    map<string, GLint>::iterator  it = uniform.find(name);
    if (it == uniform.end())
    {
        // not find
        LOG( "not found uniform by name:[%s]" , name.c_str() );
        return -1;
    }
    return it->second;
}
