//
//  GLObject2D.cpp
//  GLRender
//
//  Created by dinosaur on 13-11-28.
//  Copyright (c) 2013 dinosaur. All rights reserved.
//

#include "GLProgram.h"
#include "GLObject.h"
#include "GLTexture.h"
#include "GLSupport.h"
#include "kazmath.h"


// release object
// did not release the texture
GLObject2D::~GLObject2D()
{
    
}


GLObject2D::GLObject2D(const kmVec2 size,const kmVec2 pos)
{
    _boundingBox.x = size.x;
    _boundingBox.y = size.y;
    _boundingBox.z = .0;
    
    _texture = nullptr;
    this->initTexCoord();
}

GLObject2D::GLObject2D(float posX, float posY,float w, float h)
{
    _boundingBox.x = w;
    _boundingBox.y = h;
    _boundingBox.z = .0;
    _texture = nullptr;
    this->initTexCoord();
}


GLObject2D::GLObject2D(Color4B start, Color4B end)
{
    /*
     3      4 // START
     
     1      2 // END
     */
    _quadInfo.lu.color = start;
    _quadInfo.ru.color = start;
    
    _quadInfo.ld.color = end;
    _quadInfo.rd.color = end;
    
    _boundingBox.x = GLSupport::frameBufferWidth();
    _boundingBox.y = GLSupport::frameBufferHeight();
    _boundingBox.z = .0;
    
    _texture = nullptr;
    this->initTexCoord();
}

void GLObject2D::transform()
{
    if(_needRecomputeTransform)
        this->initPosition();
    
    // call supper to update matrix;
    GLObjectBase::transform();
}

void GLObject2D::setSize(float w, float h)
{
    if(kmAlmostEqual(w, _boundingBox.x) && kmAlmostEqual(h, _boundingBox.y)) return;
    
    _boundingBox.x = w;
    _boundingBox.y = h;
    _boundingBox.z = .0;
    
    _needRecomputeTransform = true;
}

void GLObject2D::setSize(kmVec2 sv)
{
    GLObject2D::setSize(sv.x, sv.y);
}

kmVec2 GLObject2D::size()
{
    return {_boundingBox.x, _boundingBox.y};
}

void GLObject2D::setTexture(GLTexture * tex)
{
    if(_texture == tex) return;
    _texture = tex;
    this->initTexCoord();
}

GLTexture * GLObject2D::getTexture()
{
    return _texture;
}

void GLObject2D::rotateZ(float angle)
{
    _rotate.z = angle;
    _needRecomputeTransform = true;
}


#pragma mark -
////////////////////////////////////////////////////////////////////////



void GLObject2D::setColor(Color4B start, Color4B end)
{
    _quadInfo.lu.color = start;
    _quadInfo.ru.color = start;
    
    _quadInfo.ld.color = end;
    _quadInfo.rd.color = end;
}
void GLObject2D::setColor(Color4B color)
{
    _quadInfo.lu.color = color;
    _quadInfo.ru.color = color;
    
    _quadInfo.ld.color = color;
    _quadInfo.rd.color = color;
}

void GLObject2D::setStartColor(Color4B color)
{
    _quadInfo.lu.color = color;
    _quadInfo.ru.color = color;
}

void GLObject2D::setEndColor(Color4B color)
{
    _quadInfo.ld.color = color;
    _quadInfo.rd.color = color;
}

// 随时针 坐标
void GLObject2D::initPosition()
{
    // left-down
    _quadInfo.ld.vertices[0] = _position.x;
    _quadInfo.ld.vertices[1] = _position.y;
    
    // left-up
    _quadInfo.lu.vertices[0] = _position.x;
    _quadInfo.lu.vertices[1] = _position.y + _boundingBox.y;
    
    
    // right-up
    _quadInfo.ru.vertices[0] = _position.x + _boundingBox.x;
    _quadInfo.ru.vertices[1] = _position.y + _boundingBox.y;
    
    // right-down
    _quadInfo.rd.vertices[0] = _position.x + _boundingBox.x;
    _quadInfo.rd.vertices[1] = _position.y;
}


void GLObject2D::initTexCoord()
{
    if(_texture == nullptr) return;
    
    int tw = _texture->maxT();
    int th = _texture->maxS();
    kmVec4 frame = _texture->frame();
    
    // left-down
    _quadInfo.ld.texCoords[0] = frame.x / tw;
    _quadInfo.ld.texCoords[1] = frame.y / th;
    
    // left-up
    _quadInfo.lu.texCoords[0] = frame.x / tw;;
    _quadInfo.lu.texCoords[1] = (frame.y + frame.w) / th;
    
    
    // right-up
    _quadInfo.ru.texCoords[0] = (frame.x + frame.z) / tw;
    _quadInfo.ru.texCoords[1] = (frame.y + frame.w) / th;
    
    // right-down
    _quadInfo.rd.texCoords[0] = (frame.x + frame.z) / tw;
    _quadInfo.rd.texCoords[1] = frame.y / th;
}

void GLObject2D::visit(){ Exception("WARNNING:", "overwrite me"); }
void GLObject2D::draw(){ Exception("WARNNING:", "overwrite me"); }
void GLObject2D::usePorgram(){ Exception("WARNNING:", "overwrite me"); }

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#pragma mark GLObjectContainer
void GLObjectContainer::usePorgram(){ Exception("WAINING:", "GLObject2DContainer did not need this"); }
void GLObjectContainer::visit()
{
    // I'm just a container
    // do not need to draw anything.
    // so, just update transform is OK;
    this->transform();
}

void GLObjectContainer::draw()
{
    // does need to do nothing?
    if( _alpha < 0.0001 || _hidden ) return;
    
    // OK...
    this->visit();
    
    if(_children)
    {
        for (list<GLObjectBase *>::iterator it = _children->begin(); it != _children->end(); ++it)
        {
            (*it)->draw();
        }
    }
}

#pragma mark GLSprite
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void GLSprite::usePorgram()
{
    UsingProgram(GLProgram::defaultTextureDrawProgram()->programID());
    
    GLint t;
    if( (t=GLProgram::defaultTextureDrawProgram()->uniformForName(OBJ_MATRIX)) !=-1 )
    {
        glUniformMatrix4fv(t, 1, 0,this->_transformMatrix.mat);
    }
    
    if( (t=GLProgram::defaultTextureDrawProgram()->uniformForName(PROJECT_MATRIX)) !=-1 )
    {
        glUniformMatrix4fv(t, 1, 0, GLSupport::projectionMatrix->mat);
    }
    
    if( (t=GLProgram::defaultTextureDrawProgram()->uniformForName(MV_MATRIX)) !=-1 )
    {
        glUniformMatrix4fv(t, 1, 0, GLSupport::modelViewMatrix->mat);
    }
    
    if( (t=GLProgram::defaultTextureDrawProgram()->uniformForName(ALPHA)) !=-1 )
    {
        glUniform1f(t, this->getAlphaValue());
    }
}

void GLSprite::visit()
{
    this->transform();
    this->usePorgram();
    _blend.blend();

    if(_texture)
        _texture->bind();
    
    int strike = sizeof(PerPointInfo);
	char * startAddr = (char *)&_quadInfo;
    
    glEnableVertexAttribArray( GLProgram::ATTRIB_VERTEX );
    glVertexAttribPointer(GLProgram::ATTRIB_VERTEX, 2,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)startAddr);
    
    
    glVertexAttribPointer(GLProgram::ATTRIB_TEXCOORD, 2,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)(startAddr + sizeof(float) * 2));
    glEnableVertexAttribArray( GLProgram::ATTRIB_TEXCOORD );
    
#ifdef DEBUG
        if(_texture==nullptr)
            LOG("sprite has no texture.<%p>",this);
#endif
    
    // disable color
    // sprite do not draw backgound
    glDisableVertexAttribArray(GLProgram::ATTRIB_COLOR);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void GLSprite::draw()
{
    if( _alpha < 0.0001 || _hidden ) return;
    this->visit();
    
    if(!_children) return;
    
    for (list<GLObjectBase *>::iterator it = _children->begin(); it != _children->end(); ++it) {
        (*it)->draw();
    }
}

#pragma mark GLColorLayer
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void GLColorLayer::usePorgram()
{
    UsingProgram(GLProgram::defaultColorDrawProgram()->programID());
    
    GLint t;
    if( (t=GLProgram::defaultColorDrawProgram()->uniformForName(OBJ_MATRIX)) !=-1 )
    {
        glUniformMatrix4fv(t, 1, 0,this->_transformMatrix.mat);
    }
    
    if( (t=GLProgram::defaultColorDrawProgram()->uniformForName(PROJECT_MATRIX)) !=-1 )
    {
        glUniformMatrix4fv(t, 1, 0, GLSupport::projectionMatrix->mat);
    }
    
    if( (t=GLProgram::defaultColorDrawProgram()->uniformForName(MV_MATRIX)) !=-1 )
    {
        glUniformMatrix4fv(t, 1, 0, GLSupport::modelViewMatrix->mat);
    }
    
    if( (t=GLProgram::defaultColorDrawProgram()->uniformForName(ALPHA)) !=-1 )
    {
        glUniform1f(t, this->getAlphaValue());
    }
}

void GLColorLayer::visit()
{
    this->transform();
    this->usePorgram();
    _blend.blend();
    
    int strike = sizeof(PerPointInfo);
	char * startAddr = (char *)&_quadInfo;
    
    glVertexAttribPointer(GLProgram::ATTRIB_VERTEX, 2,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)startAddr);
    glEnableVertexAttribArray( GLProgram::ATTRIB_VERTEX );
    
    glVertexAttribPointer(GLProgram::ATTRIB_COLOR, 4,
                          GL_UNSIGNED_BYTE, GL_TRUE,
                          strike, (void*)(startAddr + sizeof(float) * 4));
    glEnableVertexAttribArray( GLProgram::ATTRIB_COLOR );
    
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLColorLayer::draw()
{
    if( _alpha < 0.0001 || _hidden ) return;
    this->visit();
    
    if(_children)
    {
        for (list<GLObjectBase *>::iterator it = _children->begin(); it != _children->end(); ++it)
        {
            (*it)->draw();
        }
    }
}


