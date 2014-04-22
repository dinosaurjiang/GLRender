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

using namespace std;

GLMutableLineLabel::~GLMutableLineLabel()
{
    GLTextureManager::deleteTexture(_texture);
}


GLMutableLineLabel::GLMutableLineLabel(string & text,float fontSize,string & fontName, Color4B color,GLHTextAlignment halign,GLVTextAlignment valign,GLLineBreakMode breakMode,float width, float height)
{
    TextureInfo textTexture = createGLTextureWithString(text,
                                                        fontSize,
                                                        fontName,
                                                        halign,
                                                        valign,
                                                        breakMode,
                                                        width,
                                                        height);
    // create texture
    // here we do not cache gltexture
    // just keep it,
    // and when label delete, also delete texture.
    _texture = new GLTexture(textTexture);
    
    _boundingBox.x = textTexture.width;
    _boundingBox.y = textTexture.height;
    _boundingBox.z = 0;
    _position = {0};
    
    
    this->initPosition();
    this->initTexCoord();
    
    
    _quadInfo.lu.color = color;
    _quadInfo.ru.color = color;
    
    _quadInfo.ld.color = color;
    _quadInfo.rd.color = color;
}

GLMutableLineLabel::GLMutableLineLabel(string & text,float fontSize,string & fontName, Color4B color,float width, float height)
{
    GLMutableLineLabel::GLMutableLineLabel(text,fontSize,fontName,color,
                     kGLHTextAlignmentCenter,kGLVTextAlignmentTop,kGLLineBreakModeClip,width,height);
}


void GLMutableLineLabel::usePorgram()
{
    GLSprite::usePorgram();
    
    GLuint tt;
    if( (tt=GLProgram::defaultProgram()->uniformForName(PIX_MODE)) !=-1 )
    {
        glUniform1f(tt, 10);////// for drawing mode, blend text and color
    }
}

void GLMutableLineLabel::visit()
{
    this->transform();
    this->usePorgram();
    _blend.blend();
    if(_texture)_texture->bind();
    
    int strike = sizeof(PerPointInfo);
	char * startAddr = (char *)&_quadInfo;
    
#ifdef DEBUG
    if(_texture==nullptr)
        LOG("glsprite has no texture.<%p>",this);
#endif
    
    glEnableVertexAttribArray( ATTRIB_VERTEX );
    glVertexAttribPointer(ATTRIB_VERTEX, 2,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)startAddr);
    
    glVertexAttribPointer(ATTRIB_TEXCOORD, 2,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)(startAddr + sizeof(float) * 2));
    glEnableVertexAttribArray( ATTRIB_TEXCOORD );
    
    
    // use color for text color
    glVertexAttribPointer(ATTRIB_COLOR, 4,
                          GL_UNSIGNED_BYTE, GL_TRUE,
                          strike, (void*)(startAddr + sizeof(float) * 4));
    glEnableVertexAttribArray( ATTRIB_COLOR );
    
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
#pragma mark -

GLLabel::~GLLabel()
{
//    GLMutableLineLabel::~GLMutableLineLabel();
}

GLLabel::GLLabel(string & text,float fontSize,string & fontName, Color4B color,GLHTextAlignment halign,GLVTextAlignment valign,GLLineBreakMode breakMode):GLMutableLineLabel(text,fontSize,fontName,color,halign,valign,breakMode,.0,.0)
{
    
}

GLLabel::GLLabel(string & text,float fontSize,string & fontName, Color4B color):GLMutableLineLabel(text,fontSize,fontName,color,.0,.0)
{
    
}

