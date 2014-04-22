//
//  GLObjectBase.cpp
//  GLRender
//
//  Created by dinosaur on 13-12-25.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

#include "GLObjectBase.h"
#include "GLRenderDef.h"
#include "Utilities.h"
#include "vec4.h"


/*
 * compute anchor point as (0,0,0)
 *  1. rotate the matrix.
 *  2. scale the matrix.
 *  3. translate to world matrix
 */

void GLObjectBase::updateTransform()
{
    // When self matrix has been modify. it may affect the child
    // so, will make children to change
    // first convert from parents
    // then make children convert from self
    
    // multiply the parent
    if(_parent)
        kmMat4Multiply(&_transformMatrix, &(this->_parent->_transformMatrix),&_transformMatrix);
    
    // update child
    if(_children == nullptr) return;
    for (list<GLObjectBase *>::iterator it = _children->begin(); it != _children->end(); ++it)
    {
        (*it)->updateTransform();
    }
}

void GLObjectBase::transform()
{
    if (_needRecomputeTransform)
    {
        // this is identify matrix.
        // there may call forquntily
        // so make this function run as faster as it can.
        kmMat4 origin;
        kmVec3 center = this->getAnchorPoint();
        kmMat4Translation(&origin, -center.x, -center.y, -center.z);
        
        
        bool rotateFlag = false, scaleFlag = false;
        
        // scale
        if(!(kmAlmostEqual(_scale.x, 1.0) && kmAlmostEqual(_scale.y, 1.0)))
        {
            kmMat4 scale;
            kmMat4Scaling(&scale, _scale.x, _scale.y,1.0);
            kmMat4Multiply(&origin, &scale, &origin);
            scaleFlag = true;
        }
        
        //rotate
        kmMat4 RX,RY,RZ;
        if (!kmAlmostEqual(_rotate.x,0.0))
        {
            kmMat4RotationX(&RX, _rotate.x);
            kmMat4Multiply(&origin, &RX, &origin);
            rotateFlag = true;
        }
        
        if (!kmAlmostEqual(_rotate.y,0.0))
        {
            kmMat4RotationY(&RY, _rotate.y);
            kmMat4Multiply(&origin, &RY, &origin);
            rotateFlag = true;
        }
        
        if (!kmAlmostEqual(_rotate.z,0.0))
        {
            kmMat4RotationZ(&RZ, _rotate.z);
            kmMat4Multiply(&origin, &RZ, &origin);
            rotateFlag = true;
        }
        
        if (rotateFlag || scaleFlag)
        {
            // translate to the points
            kmMat4 trans;
            kmMat4Translation(&trans, center.x, center.y, center.z);
            kmMat4Multiply(&_transformMatrix, &trans, &origin);
        }
        else
        {
            kmMat4Identity(&_transformMatrix);
        }
        
        // convert from parents matrix
        this->updateTransform();
        
        _needRecomputeTransform = false;
    }
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
GLObjectBase::GLObjectBase()
{
    
}

GLObjectBase::~GLObjectBase()
{
    if (_children)
    {
        //relase the children.
        for (list<GLObjectBase *>::iterator it = _children->begin(); it != _children->end(); ++it)
        {
            GLObjectBase * temp = (GLObjectBase *)(*it);// get
            delete temp;// delete child
            //_children->erase(it);// erase from list
        }
        delete _children;
        _children = nullptr;// to keep safe.
    }
    
    LOG("GLObjectBase::~GLObjectBase<%p>", this );
}


void GLObjectBase::setAnchor(float x, float y, float z)
{
    _anchorValue.x = x;
    _anchorValue.y = y;
    _anchorValue.z = z;
    _needRecomputeTransform = true;
}

kmVec3 GLObjectBase::getAnchor()
{
    return _anchorValue;
}

kmVec3 GLObjectBase::getAnchorPoint()
{
    kmVec3 t;
    t.x = _position.x + _boundingBox.x * _anchorValue.x;
    t.y = _position.y + _boundingBox.y * _anchorValue.y;
    t.z = _position.z + _boundingBox.z * _anchorValue.z;
    return t;
}


void GLObjectBase::setPosition(kmVec3 pos)
{
    GLObjectBase::setPosition(pos.x , pos.y, pos.z);
}

void GLObjectBase::setPosition(float x, float y)
{
    GLObjectBase::setPosition(x,y,_position.z);
}

void GLObjectBase::setPosition(float x, float y, float z)
{
    _position.x = x;
    _position.y = y;
    _position.z = z;
    
    _needRecomputeTransform = true;
}


void GLObjectBase::setRotate(kmVec3 value)
{
    GLObjectBase::setRotate(value.x , value.y, value.z);
}

void GLObjectBase::setRotate(float x, float y, float z)
{
    // rotate matrix is expensive,
    // so we need to know,weather the rotation is need.
    if(kmAlmostEqual(_rotate.x , x) &&
       kmAlmostEqual(_rotate.y , y) &&
       kmAlmostEqual(_rotate.z , z)) return;
    
    _rotate.x = x;
    _rotate.y = y;
    _rotate.z = z;
    
    _needRecomputeTransform = true;
}

void GLObjectBase::setScale(kmVec3 value)
{
    GLObjectBase::setScale(value.x , value.y, value.z);
}


void GLObjectBase::setScale(float x, float y, float z)
{
    if(kmAlmostEqual(_scale.x , x) &&
       kmAlmostEqual(_scale.y , y) &&
       kmAlmostEqual(_scale.z , z)) return;
    
    _scale.x = x;
    _scale.y = y;
    _scale.z = z;
    
    _needRecomputeTransform = true;
}


void GLObjectBase::update(float dt)
{
    this->draw();
}


#pragma mark -
void GLObjectBase::setZDepth(int tv)
{
    GLAssert(tv > -2, "GLObjectBase::zdepth can not < -1\n");
    _zDepth = tv;
}

/*
 *  关于zdepth 的问题。
 *  其实刚开始这个只是用来2d的  所以用了zdepth 这个属性。
 *  如果zdepth 不是 -1 的话   就按照zdepth 来排序。 
 *  大的zdepth 在后面，front of list 是最小的。
 *  =====================================================
 *  但是对于3D的话，其实是不需要z depth的。因为他有 z 坐标。
 *  这里zdepth 就默认-1 就OK了。
 *  绘制的时候，都是从小到大 遍历  然后绘制的
 */

//TODO: test and finish me.
//TODO: 要是已经有这个object了  怎么办。
void GLObjectBase::addChild(GLObjectBase * child)// default z depth -1;
{
    if(_children == nullptr)
    {
        _children = new list<GLObjectBase *>();
    }
    
    // init preperty
    child->_parent = this;
    child->_needRecomputeTransform = true;
    
    int zDepth = child->getZDepth();
    
    if (zDepth == -1)
    {
        //最小的 不管。直接加在最前面。
        _children->push_front(child);
    }
    else
    {
        list<GLObjectBase *>::iterator it = _children->end();
        while (it != _children->begin() && (*it)->_zDepth > zDepth)
        {
            it--;
        }
        
        // 这个要insert 到it的前面。
        // 但是it的z < child.z
        // 所以要++一下
        it++;
        _children->insert(it, child);
    }
}

void GLObjectBase::changeZDepthOfChildByTag(int zValue, int tv)
{
    GLObjectBase * obj = this->removeChildByTag(tv,false);
    obj->setZDepth(zValue);
    this->addChild(obj);
}

void GLObjectBase::removeAllChildren(bool clean)
{
    if (!clean)
    {
        // do not want clean.
        // so, just remove the list is OK.
        delete _children;
        _children = nullptr;// to keep safe.
        return;
    }
    
    
    if (_children)
    {
        //relase the children.
        for (list<GLObjectBase *>::iterator it = _children->begin();
             it != _children->end();
             ++it)
        {
            // get
            GLObjectBase * temp = (GLObjectBase *)(*it);
            temp->removeAllChildren(clean);
            
            delete temp;// delete child
        }
        
        // also delete list.
        // if new child added.
        // list will auto alloc.
        delete _children;
        _children = nullptr;// to keep safe.
    }
}

GLObjectBase * GLObjectBase::removeChildByTag(int tv,bool clean)
{
    if(!_children) return nullptr;
    
    // find that one.
    GLObjectBase * tmp = nullptr;
    for (list<GLObjectBase *>::iterator it = _children->begin(); it != _children->end(); ++it) {
        if ((*it)->_tag == tv)
        {
            tmp = *it;
            tmp->_parent = nullptr;
            tmp->_needRecomputeTransform = true;
            _children->erase(it);
            break;
        }
    }
    
    // if find and want to delete it
    if(clean && tmp)
    {
        delete tmp;
        return nullptr;
    }
    return tmp;
}

bool GLObjectBase::pointInside(kmVec2 pos2d)
{
    return GLObjectBase::pointInside(pos2d.x, pos2d.y);
}


//TODO: 点-矩阵变换
bool GLObjectBase::pointInside(float x, float y)
{
//    kmVec4 org = {x,y,0,1.0};
//    kmVec4 t;
//    kmVec4Transform(&t, &org, &_transformMatrix);

    kmVec2 t = {x, y};
    if (t.x > _position.x &&
        t.y > _position.y &&
        t.x < _position.x + _boundingBox.x &&
        t.x < _position.y + _boundingBox.y)
    {
        return true;
    }
    return false;
}

bool GLObjectBase::pointInside(kmVec3 pos3d)
{
    return GLObjectBase::pointInside(pos3d.x, pos3d.y, pos3d.z);
}

bool GLObjectBase::pointInside(float x, float y, float z)
{
//    kmVec4 org = {x,y,z,1.0};
//    kmVec4 t;
//    kmVec4Transform(&t, &org, &_transformMatrix);

    kmVec3 t = {x, y, z};
    
    if (t.x > _position.x &&
        t.y > _position.y &&
        t.z > _position.z &&
        t.x < _position.x + _boundingBox.x &&
        t.x < _position.y + _boundingBox.y &&
        t.x < _position.z + _boundingBox.z)
    {
        return true;
    }
    return false;
}

GLObjectBase * GLObjectBase::hitTest(kmVec2 pos2d)
{
    return GLObjectBase::hitTest(pos2d.x, pos2d.y);
}

GLObjectBase * GLObjectBase::hitTest(float x, float y)
{
    // first check if child hited
    for (list<GLObjectBase *>::iterator it = _children->begin(); it != _children->end(); ++it)
    {
        GLObjectBase * temp = (*it)->hitTest(x, y);
        if(temp) return temp;
    }
    
    
    if (this->pointInside(x,y))
    {
        return this;
    }
    
    
    return nullptr;
}

GLObjectBase * GLObjectBase::hitTest(kmVec3 pos3d)
{
    return GLObjectBase::hitTest(pos3d.x, pos3d.y,pos3d.z);
}

GLObjectBase * GLObjectBase::hitTest(float x, float y, float z)
{
    // first check if child hited
    for (list<GLObjectBase *>::iterator it = _children->begin(); it != _children->end(); ++it)
    {
        GLObjectBase * temp = (*it)->hitTest(x, y,z);
        if(temp) return temp;
    }
    
    
    if (this->pointInside(x,y,z))
    {
        return this;
    }
    return nullptr;
}


