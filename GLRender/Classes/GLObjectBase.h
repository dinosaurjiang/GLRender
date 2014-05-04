//
//  GLObjectBase.h
//  GLRender
//
//  Created by dinosaur on 13-12-25.
//  Copyright (c) 2013 dinosaur. All rights reserved.
//

#ifndef __GLRender__GLObjectBase__
#define __GLRender__GLObjectBase__


#include <iostream>
#include <vector>
#include <list>
#include "kazmath.h"
#include "GLBlender.h"
#include "GLBase.h"

/*
 *  这个是个基础类
 *  是所有对象的基础。
 *  它就是一个抽象类，
 *  很多方法只是声明了，并不实现，需要子类实现
 *  其实还是实现了部分方法，大多就是更新属性的那种。
 */

using namespace std;


#ifdef __cplusplus
extern "C" {
#endif

    typedef struct kmVec3 BoundingBox;
    
#ifdef __cplusplus
}
#endif




class GLObjectBase : public GLBase
{
public:
    
    virtual ~GLObjectBase();
    GLObjectBase();
    
    DECLARE_CLASS(GLObjectBase)
    
    // just declare
    // not implements
    // because ..I do't know how to draw
    virtual void visit()=0;
    virtual void draw()=0;
    virtual void usePorgram()=0;
    
    /* 
     * some object need to update itself.
     * just invoke draw methods at here.
     * if you have something to update.
     * overwrite me.
     */
    virtual void update(float dt);
    
    // en, you understand
    void addChild(GLObjectBase * child);// order by _zDepth;
    void changeZDepthOfChildByTag(int zValue, int tv);
    GLObjectBase * removeChildByTag(int tv ,bool clean);
    void removeAllChildren(bool clean);
    
    
    //////////////////////////////////////////
    //////////// getter and setter ///////////
    void setTag(int tv){ _tag = tv; };
    int getTag(){ return _tag; };
    
    
    void setParent(GLObjectBase *obj){ _parent = obj; };
    GLObjectBase * getParent(){ return _parent; };
    
    
    void setHidden(bool v){ _hidden = v; };
    bool isHidden(){ return _hidden; };
    
    
    kmMat4 * transformMatrixPtr(){ return &_transformMatrix; };
    kmMat4 transformMatrix(){ return _transformMatrix; };
    
    void setAlphaValue(float v) { _alpha = v < 0.0002 ? 0.0 : (v > 1.0 ? 1.0 : v); };
    float getAlphaValue(){ return _alpha; };
    
    void setZDepth(int tv);
    int getZDepth(){ return _zDepth; };
    
    
    // this may override by subclass
    virtual void setAnchor(float x, float y, float z);
    kmVec3 getAnchor();
    kmVec3 getAnchorPoint();
    
    
    virtual void setPosition(kmVec3 pos);
    virtual void setPosition(float x, float y); // default keep z value
    virtual void setPosition(float x, float y, float z);
    kmVec3 getPosition(){ return _position; };

    
    virtual void setRotate(kmVec3 value);
    virtual void setRotate(float x, float y, float z);
    kmVec3 getRotate(){ return _rotate; };
    kmVec3 * getRotatePtr(){ return &_rotate; };
    
    
    virtual void setScale(kmVec3 value);
    virtual void setScale(float x, float y, float z);
    kmVec3 getScale(){ return _scale; };
    
    
    //////////////////////////////
    // for pick up
    // TODO: not finished.
    // warnning: can not use now.
    //////////////////////////////
    bool pointInside(kmVec2 pos2d);
    bool pointInside(float x, float y);
    bool pointInside(kmVec3 pos3d);
    bool pointInside(float x, float y, float z);
    
    
    GLObjectBase * hitTest(kmVec2 pos2d);
    GLObjectBase * hitTest(float x, float y);
    GLObjectBase * hitTest(kmVec3 pos3d);
    GLObjectBase * hitTest(float x, float y, float z);
    
protected:
    
    virtual void transform();
    virtual void updateTransform();
    
    // for c++11, can init here.
    int                         _tag = -1;
    GLObjectBase*               _parent = nullptr;
    bool                        _hidden = false;
    kmMat4                      _transformMatrix;
    
    
    list<GLObjectBase *> *      _children = nullptr;
    
    float                       _alpha = 1.0;
    int                         _zDepth = -1;
    kmVec3                      _anchorValue = {0.5,0.5,0.0};
    
    // front-left-down  is (0,0,0)
    kmVec3                      _position = {0};
    kmVec3                      _rotate = {0};
    kmVec3                      _scale = {1.0,1.0,1.0};
    BoundingBox                 _boundingBox = {0.0};
    bool                        _needRecomputeTransform = true;
    
private:
    
};


#endif /* defined(__GLRender__GLObjectBase__) */
