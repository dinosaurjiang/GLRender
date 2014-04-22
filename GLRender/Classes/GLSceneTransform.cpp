//
//  GLSceneTransform.cpp
//  GLRender
//
//  Created by dinosaur on 13-12-26.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

#include "GLSceneTransform.h"
#include "GLScene.h"
#include "GLSupport.h"
#include "GLTexture.h"
#include "GLProgram.h"
#include "GLBlender.h"
#include "GLOfflineRender.h"
#include "GLRenderDef.h"
#include "Utilities.h"

GLSceneTransform::~GLSceneTransform()
{
    if (_fromQuadList)
    {
        delete [] _fromQuadList;
    }
    
    if (_toQuadList)
    {
        delete [] _toQuadList;
    }
    
    if (_fromTexture)
    {
        GLTextureManager::deleteTexture(_fromTexture);
        _fromTexture = nullptr;
    }
    
    if (_toTexture)
    {
        GLTextureManager::deleteTexture(_toTexture);
        _toTexture = nullptr;
    }
}

GLSceneTransform::GLSceneTransform(GLScene * from, GLScene * to, int fquadCnt,int tquadCnt,bool fromFirst)
{
    _fromAlpha = 1.0;
    _fromQuadCount = fquadCnt;
    _fromScene = from;
    
    _toAlpha = 1.0;
    _toQuadCount = tquadCnt;
    _toScene = to;
    
    
    _fromFirstDraw = fromFirst;
    _isStop = false;
}

void GLSceneTransform::drawFromTexture()
{
    // nothing to do
    if ( _fromQuadCount < 1 || _fromQuadList == nullptr ) return;
    
    GLint t;
    if( (t=GLProgram::defaultProgram()->uniformForName(ALPHA)) !=-1 )
        glUniform1f(t, this->_fromAlpha);
    
    
    if(_fromTexture)_fromTexture->bind();
    
    int strike = sizeof(PerPointInfo);
	char * startAddr = (char *)_fromQuadList;
    
    glEnableVertexAttribArray( ATTRIB_VERTEX );
    glVertexAttribPointer(ATTRIB_VERTEX, 3,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)startAddr);
    
    
    glVertexAttribPointer(ATTRIB_TEXCOORD, 2,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)(startAddr + sizeof(float) * 3));
    glEnableVertexAttribArray( ATTRIB_TEXCOORD );
#ifdef DEBUG
    if(_fromTexture==nullptr)
        LOG("glsprite has no texture.<%p>",this);
#endif
    
    // disable color
    // sprite do not draw backgound
    glDisableVertexAttribArray(ATTRIB_COLOR);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLSceneTransform::drawToTexture()
{
    // nothing to do
    if (_toQuadCount < 1 || _toQuadList == nullptr) return;
    
    GLint t;
    if( (t=GLProgram::defaultProgram()->uniformForName(ALPHA)) !=-1 )
        glUniform1f(t, this->_toAlpha);
    
    
    this->usePorgram();
    if(_toTexture)_toTexture->bind();
    
    int strike = sizeof(PerPointInfo);
	char * startAddr = (char *)_toQuadList;
    
    glEnableVertexAttribArray( ATTRIB_VERTEX );
    glVertexAttribPointer(ATTRIB_VERTEX, 3,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)startAddr);
    
    
    glVertexAttribPointer(ATTRIB_TEXCOORD, 2,
                          GL_FLOAT, GL_FALSE,
                          strike, (void*)(startAddr + sizeof(float) * 3));
    glEnableVertexAttribArray( ATTRIB_TEXCOORD );
#ifdef DEBUG
    if(_toTexture==nullptr)
        LOG("glsprite has no texture.<%p>",this);
#endif
    
    // disable color
    // sprite do not draw backgound
    glDisableVertexAttribArray(ATTRIB_COLOR);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLSceneTransform::visit()
{
    this->transform();
    this->usePorgram();
    
    // blend
    _blend.blend();
    
    // draw first.
    if(_fromFirstDraw)
        this->drawFromTexture();
    else
        this->drawToTexture();
    
    // blend
    _blend.blend();
    
    
    // draw second
    if(_fromFirstDraw)
        this->drawToTexture();
    else
        this->drawFromTexture();
}

void GLSceneTransform::draw()
{
    this->visit();
}

void GLSceneTransform::usePorgram()
{
    UsingProgram(GLProgram::defaultProgram()->programID());
    
    GLint t;
    if( (t=GLProgram::defaultProgram()->uniformForName(OBJ_MATRIX)) !=-1 )
    {
        glUniformMatrix4fv(t, 1, 0,this->_transformMatrix.mat);
    }
    
    if( (t=GLProgram::defaultProgram()->uniformForName(PROJECT_MATRIX)) !=-1 )
    {
        glUniformMatrix4fv(t, 1, 0, GLSupport::projectionMatrix->mat);
    }
    
    if( (t=GLProgram::defaultProgram()->uniformForName(MV_MATRIX)) !=-1 )
    {
        glUniformMatrix4fv(t, 1, 0, GLSupport::modelViewMatrix->mat);
    }
    
    if( (t=GLProgram::defaultProgram()->uniformForName(PIX_MODE)) !=-1 )
    {
        glUniform1f(t, 0);////// for drawing mode
    }
}

void GLSceneTransform::update(float dt)
{
    if (!_isReady)
    {
        // init
        this->getReady();
        this->initTextures();
        this->updateCoordinate(0);
        _isReady = true;
        
        return;
    }
    
    // stopped, do nothing.
    if(_isStop)
        return;
    
    
    _curTime+=dt;
    if (_curTime > _duration)
    {
        ////////////////////////////////////
        // 最后一次更新数值
        // 这是为了消除误差。
        float value = timing_function_value_with_name(timingFunction, 1.0);
        this->update(value);
        ////////////////////////////////////
        _isStop = true;
        return;
    }
    
    // 归一化到 [0,1]
    float tvalue = (_curTime / _duration);
    float value = timing_function_value_with_name(timingFunction, tvalue);
    this->updateCoordinate(value);
    _lastValueFlag = value;
}


void GLSceneTransform::getReady()
{
    // alloc memory
    if (_fromQuadCount > 0)
    {
        _fromQuadList = new  Transform3DQuad[_fromQuadCount];
    }
    GLAssert(_fromTexture == nullptr, "alloc mem for from list error\n");
    
    if (_toQuadCount > 0)
    {
        _toQuadList = new  Transform3DQuad[_toQuadCount];
    }
    GLAssert(_toQuadList == nullptr, "alloc mem for to list error\n");
}


void GLSceneTransform::initTextures()
{
    // gen texture from scene
    float ww,hh;
    GLSupport::openGLViewSize(&ww, &hh);
    GLOfflineRender * render = new GLOfflineRender(ww,hh);
    
    // get from
    render->begin();
    _fromScene->draw();
    render->end();
    _fromTexture = render->renderToTexture();
    
    // get to.
    render->begin();
    _toScene->draw();
    render->end();
    _toTexture = render->renderToTexture();
    
    
    delete render;
}


//OVERWRITE ME
void GLSceneTransform::updateCoordinate(float dt)
{
    Exception("Need Overwrite", "GLSceneTransform::updateCoordinate need be overwrite\n");
}

#pragma mark -
#pragma mark GLTransformAppearFrom
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


static inline void updateQuadTexCoord(Transform3DQuad * quad, kmVec2 coord[4])
{
    kmVec2Assign(&quad->t1p1.UV, coord);//1
    kmVec2Assign(&quad->t1p2.UV, coord+1);//2
    kmVec2Assign(&quad->t1p3.UV, coord+3);//4
    
    kmVec2Assign(&quad->t2p1.UV, coord+1);//2
    kmVec2Assign(&quad->t2p2.UV, coord+3);//4
    kmVec2Assign(&quad->t2p3.UV, coord+2);//3
}

static inline void updateQuadCoord(Transform3DQuad * quad, kmVec3 coord[4])
{
    kmVec3Assign(&quad->t1p1.XYZ, coord);//1
    kmVec3Assign(&quad->t1p2.XYZ, coord+1);//2
    kmVec3Assign(&quad->t1p3.XYZ, coord+3);//4
    
    kmVec3Assign(&quad->t2p1.XYZ, coord+1);//2
    kmVec3Assign(&quad->t2p2.XYZ, coord+3);//4
    kmVec3Assign(&quad->t2p3.XYZ, coord+2);//3
}

// FROM 保持不动。
// TO 从四个方向滑动过来。
GLTransformAppear::GLTransformAppear(GLScene * from, GLScene * to,Orientation ori):GLSceneTransform(from,to,1,1,true)
{
    _offset = 0;
    _ori = ori;
}


void GLTransformAppear::_initTexCoordinateAndOther()
{
    float ww,hh;
    GLSupport::openGLViewSize(&ww, &hh);
    
    // for from
    int max_t = _fromTexture->maxT();
    int max_s = _fromTexture->maxS();
    
    kmVec4 frame_f = _fromTexture->frame();
    
    float minx = frame_f.x / max_t;
    float miny = frame_f.y / max_s;
    
    float maxx = frame_f.z / max_t;
    float maxy = frame_f.w / max_s;
    kmVec2 from_tex_coord[4] =
    {
        {minx,miny},{maxx, miny},
        {maxx,maxy},{minx,maxy}
    };
    updateQuadTexCoord(_fromQuadList, from_tex_coord);
    
    
    // also update this
    kmVec3 from_coords[4] =
    {
        {0,0,0},{ww,0,0},
        {ww,hh,0},{0,hh,0}
    };
    updateQuadCoord(_fromQuadList, from_coords);
 
    
    /// fro to texture
    max_t = _toTexture->maxT();
    max_s = _toTexture->maxS();
    
    frame_f = _toTexture->frame();
    
    minx = frame_f.x / max_t;
    miny = frame_f.y / max_s;
    
    maxx = frame_f.z / max_t;
    maxy = frame_f.w / max_s;
    kmVec2 to_tex_coord[4] =
    {
        {minx,miny},{maxx, miny},
        {maxx,maxy},{minx,maxy}
    };
    updateQuadTexCoord(_toQuadList, to_tex_coord);
    
    //借用下
    updateQuadCoord(_toQuadList, from_coords);
    
    // 初始化 to的位置。
    kmVec3 offset = {0};
    switch (_ori) {
        case Ori_Left:offset.x = ww;break;
        case Ori_Right: offset.x = -ww; break;
        case Ori_Up: offset.y = hh;break;
        case Ori_Down: offset.y = -hh; break;
        default:
            break;
    }
    
    kmVec3Add(&_toQuadList->t1p1.XYZ, &_toQuadList->t1p1.XYZ, &offset);
    kmVec3Add(&_toQuadList->t1p2.XYZ, &_toQuadList->t1p1.XYZ, &offset);
    kmVec3Add(&_toQuadList->t1p3.XYZ, &_toQuadList->t1p1.XYZ, &offset);
    
    kmVec3Add(&_toQuadList->t2p1.XYZ, &_toQuadList->t1p1.XYZ, &offset);
    kmVec3Add(&_toQuadList->t2p2.XYZ, &_toQuadList->t1p1.XYZ, &offset);
    kmVec3Add(&_toQuadList->t2p3.XYZ, &_toQuadList->t1p1.XYZ, &offset);
    
}

///// dt [0-1]
void GLTransformAppear::updateCoordinate(float dt)
{
    float ww,hh;
    GLSupport::openGLViewSize(&ww, &hh);
    
    if (!_inited || dt < 0.0024)
    {
        // here init.
        // texture and from coords.
        this->_initTexCoordinateAndOther();
        _inited = true;
        return;
    }
    
    float _mDt = dt - _lastValueFlag;
    switch (_ori) {
        case Ori_Left:
            // just update x value.
            _toQuadList->t1p1.XYZ.x -= ww * _mDt;
            _toQuadList->t1p2.XYZ.x -= ww * _mDt;
            _toQuadList->t1p3.XYZ.x -= ww * _mDt;
            
            _toQuadList->t2p1.XYZ.x -= ww * _mDt;
            _toQuadList->t2p2.XYZ.x -= ww * _mDt;
            _toQuadList->t2p3.XYZ.x -= ww * _mDt;
            break;
        case Ori_Right:
            // just update x value.
            _toQuadList->t1p1.XYZ.x += ww * _mDt;
            _toQuadList->t1p2.XYZ.x += ww * _mDt;
            _toQuadList->t1p3.XYZ.x += ww * _mDt;
            
            _toQuadList->t2p1.XYZ.x += ww * _mDt;
            _toQuadList->t2p2.XYZ.x += ww * _mDt;
            _toQuadList->t2p3.XYZ.x += ww * _mDt;
            break;
        case Ori_Up:
            _toQuadList->t1p1.XYZ.y -= hh * _mDt;
            _toQuadList->t1p2.XYZ.y -= hh * _mDt;
            _toQuadList->t1p3.XYZ.y -= hh * _mDt;
            
            _toQuadList->t2p1.XYZ.y -= hh * _mDt;
            _toQuadList->t2p2.XYZ.y -= hh * _mDt;
            _toQuadList->t2p3.XYZ.y -= hh * _mDt;
            break;
        case Ori_Down:
            _toQuadList->t1p1.XYZ.y += hh * _mDt;
            _toQuadList->t1p2.XYZ.y += hh * _mDt;
            _toQuadList->t1p3.XYZ.y += hh * _mDt;
            
            _toQuadList->t2p1.XYZ.y += hh * _mDt;
            _toQuadList->t2p2.XYZ.y += hh * _mDt;
            _toQuadList->t2p3.XYZ.y += hh * _mDt;
        default:
            break;
    }
    
}


void GLTransformAppearFromFadeOut::updateCoordinate(float dt)
{
    // call supper first.
    GLTransformAppear::updateCoordinate(dt);
    _fromAlpha = (1.0-dt);
}


void GLTransformAppearToFadeIn::updateCoordinate(float dt)
{
    // call supper first.
    GLTransformAppear::updateCoordinate(dt);
    _fromAlpha = dt;
}



GLTransformDisappear::GLTransformDisappear(GLScene * from, GLScene * to,Orientation ori):GLTransformAppear(from,to,Ori_Unknow)
{
    _fromFirstDraw = false;
    _ori_flag = ori;
}

void GLTransformDisappear::updateCoordinate(float dt)
{
    GLTransformAppear::updateCoordinate(dt);
    float ww,hh;
    GLSupport::openGLViewSize(&ww, &hh);
    float _mDt = dt - _lastValueFlag;
    switch (_ori_flag)
    {
        case Ori_Left:
            // just update x value.
            _toQuadList->t1p1.XYZ.x += ww * _mDt;
            _toQuadList->t1p2.XYZ.x += ww * _mDt;
            _toQuadList->t1p3.XYZ.x += ww * _mDt;
            
            _toQuadList->t2p1.XYZ.x += ww * _mDt;
            _toQuadList->t2p2.XYZ.x += ww * _mDt;
            _toQuadList->t2p3.XYZ.x += ww * _mDt;
            break;
        case Ori_Right:
            // just update x value.
            _toQuadList->t1p1.XYZ.x -= ww * _mDt;
            _toQuadList->t1p2.XYZ.x -= ww * _mDt;
            _toQuadList->t1p3.XYZ.x -= ww * _mDt;
            
            _toQuadList->t2p1.XYZ.x -= ww * _mDt;
            _toQuadList->t2p2.XYZ.x -= ww * _mDt;
            _toQuadList->t2p3.XYZ.x -= ww * _mDt;
            break;
        case Ori_Up:
            _toQuadList->t1p1.XYZ.y += hh * _mDt;
            _toQuadList->t1p2.XYZ.y += hh * _mDt;
            _toQuadList->t1p3.XYZ.y += hh * _mDt;
            
            _toQuadList->t2p1.XYZ.y += hh * _mDt;
            _toQuadList->t2p2.XYZ.y += hh * _mDt;
            _toQuadList->t2p3.XYZ.y += hh * _mDt;
            break;
        case Ori_Down:
            _toQuadList->t1p1.XYZ.y -= hh * _mDt;
            _toQuadList->t1p2.XYZ.y -= hh * _mDt;
            _toQuadList->t1p3.XYZ.y -= hh * _mDt;
            
            _toQuadList->t2p1.XYZ.y -= hh * _mDt;
            _toQuadList->t2p2.XYZ.y -= hh * _mDt;
            _toQuadList->t2p3.XYZ.y -= hh * _mDt;
        default:
            break;
    }
}

