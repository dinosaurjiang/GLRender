//
//  GLMutableLineLabel.cpp
//  GLRender
//
//  Created by dinosaur on 13-12-19.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

#include "GLLabel.h"
#include "GLProgram.h"
#include "GLTexture.h"
#include "GLSupport.h"
#include "CocoaTextRenderSupport.h"

using namespace std;

GLMutableLineLabel::~GLMutableLineLabel()
{
    
}


bool GLMutableLineLabel::init(string & text,string & fontName,float fontSize, Color4B color,GLHTextAlignment halign,GLVTextAlignment valign,GLLineBreakMode breakMode,float width, float height)
{
    return this->init(text.c_str(),
                      fontName.c_str(),
                      fontSize,
                      color,
                      halign,
                      valign,
                      breakMode,
                      width,
                      height);
}



bool GLMutableLineLabel::init(const char * text,
                              const char * fontName,
                              float fontSize,
                              Color4B color,
                              GLHTextAlignment halign,
                              GLVTextAlignment valign,
                              GLLineBreakMode breakMode,
                              float width,
                              float height)
{
    
    TextureInfo textTexture = createGLTextureWithString(text,
                                                        fontName,
                                                        fontSize,
                                                        halign,
                                                        valign,
                                                        breakMode,
                                                        width,
                                                        height);
    // create texture
    // here we do not cache gltexture
    // just keep it,
    // and when label delete, also delete texture.
    
    _boundingBox.x = textTexture.width;
    _boundingBox.y = textTexture.height;
    _boundingBox.z = 0;
    _position = {0};
    
    this->setTexture(new GLTexture(textTexture));
    
    _quadInfo.lu.color = color;
    _quadInfo.ru.color = color;
    
    _quadInfo.ld.color = color;
    _quadInfo.rd.color = color;
    
    _textSize = fontSize;
    _color = color;
    
    this->initPosition();
    return true;
}

bool GLMutableLineLabel::init(string & text,string & fontName,float fontSize, Color4B color,float width, float height)
{
    return this->init(text.c_str(),fontName.c_str(),fontSize,color,width,height);
}


bool GLMutableLineLabel::init( const char * text,
                              const char * fontName,
                              float fontSize,
                              Color4B color,
                              float width,
                              float height)
{
    return this->init(
                      text,fontName,fontSize,color,
                      kGLHTextAlignmentCenter,
                      kGLVTextAlignmentTop,
                      kGLLineBreakModeClip,
                      width,height
                      );
}


void GLMutableLineLabel::usePorgram()
{
    UsingProgram(GLTextureColorDrawProgram::getInstance()->programID());
    
    GLTextureColorDrawProgram::getInstance()->setupObjectMatrix(this->_transformMatrix.mat);
    GLTextureColorDrawProgram::getInstance()->setupProjectionMatrix(GLSupport::projectionMatrix->mat);
    GLTextureColorDrawProgram::getInstance()->setupModelViewMatrix(GLSupport::modelViewMatrix->mat);
    GLTextureColorDrawProgram::getInstance()->setFloat1ForUniform(DefaultUniform::GL_AlphaValue,
                                                                  this->getAlphaValue());
}

void GLMutableLineLabel::visit()
{
    this->transform();
    this->usePorgram();
    
    
    _blend.blend();
    
    this->bindTexture();
    
    int strike = sizeof(PerPointInfo);
	char * startAddr = (char *)&_quadInfo;
    
    
    glEnableVertexAttribArray(GLProgram::ATTRIB_VERTEX );
    glVertexAttribPointer(GLProgram::ATTRIB_VERTEX, 2,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)startAddr);
    
    glVertexAttribPointer(GLProgram::ATTRIB_TEXCOORD, 2,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)(startAddr + sizeof(float) * 2));
    glEnableVertexAttribArray( GLProgram::ATTRIB_TEXCOORD );
    
    
    // use color for text color
    glVertexAttribPointer(GLProgram::ATTRIB_COLOR, 4,
                          GL_UNSIGNED_BYTE, GL_TRUE,
                          strike, (void*)(startAddr + sizeof(float) * 4));
    glEnableVertexAttribArray( GLProgram::ATTRIB_COLOR );
    
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
#pragma mark -

GLLabel::~GLLabel()
{
    
}

bool GLLabel::init(string & text,string & fontName,float fontSize, Color4B color,GLHTextAlignment halign,GLVTextAlignment valign,GLLineBreakMode breakMode)
{
    return GLMutableLineLabel::init(text,fontName,fontSize,color,halign,valign,breakMode,.0,.0);
}

bool GLLabel::init(string & text,string & fontName,float fontSize, Color4B color){
    
    return GLMutableLineLabel::init(text,fontName,fontSize,color,.0,.0);
}


bool GLLabel::init(const char *text,const char *fontName,float fontSize, Color4B color,GLHTextAlignment halign,GLVTextAlignment valign,GLLineBreakMode breakMode)
{
    return GLMutableLineLabel::init(text,fontName,fontSize,color,halign,valign,breakMode,.0,.0);
}

bool GLLabel::init(const char *text,const char *fontName,float fontSize, Color4B color)
{
    return GLMutableLineLabel::init(text,fontName,fontSize,color,.0,.0);
}

