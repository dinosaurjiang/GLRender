//
//  GLObjectAction.cpp
//  GLRender
//
//  Created by dinosaur on 13-12-10.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#include "GLObjectAction.h"
#include "GLObject.h"
#include "Utilities.h"

#pragma mark base action class

GLObjectAction::GLObjectAction(GLObject2D * obj ,unsigned long actionTag, float duration)
{
    _target = obj;
    _actionTag = actionTag;
    _duration = duration;
}

unsigned long GLObjectAction::Tag()
{
    return _actionTag;
}

// 这里撒 归一话的到0-1 然后根据time func 来计算的value。
// 如果需要差值的话  需要自行计算
// 有个变量_lastValue 保存着上次的值。
void GLObjectAction::update(float dt)
{
    Exception("ABSTRACT FUNC", "OVER WRITE ME");
}

void GLObjectAction::stepIn(float dt)
{
    _curTime+=dt;
    if (_curTime > _duration)
    {
        ////////////////////////////////////
        // 最后一次更新数值
        // 这是为了消除误差。
        float value = timing_function_value_with_name(actionTimingFunction, 1.0);
        this->update(value - _lastValue);
        ////////////////////////////////////
        _stop = true;
        return;
    }
    
    // 归一化到 [0,1]
    float tvalue = (_curTime / _duration);
    float value = timing_function_value_with_name(actionTimingFunction, tvalue);
    this->update( value );
    _lastValue = value;
}

void GLObjectAction::stop()
{
    _stop = true;
}

bool GLObjectAction::isFinished()
{
    return _stop;
}

float GLObjectAction::duration()
{
    return _duration;
}

void GLObjectAction::reset2begin()
{
    _lastValue = 0.0;
    _curTime = 0.0;
    _stop = false;
}

GLObject2D * GLObjectAction::target()
{
    return _target;
}

#pragma mark -
#pragma mark GLActionCallback

GLActionCallback::GLActionCallback(unsigned long actionTag, GLActionCallback_Function func, void * ctx):GLObjectAction(nullptr,actionTag,0.0f)
{
    GLAssert(func, "GLActionCallback: can out pass NULL callback");
    _callback_func = func;
    _callback_ctx = ctx;
}

void GLActionCallback::stepIn(float dt)
{
    if (_stop == false)
    {
        _callback_func(_callback_ctx);
        _stop = true;
    }
}


#pragma mark -
#pragma mark GLActionRotateBy

GLActionRotateBy::GLActionRotateBy(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec3 angle):GLObjectAction(obj,actionTag,duration)
{
    _radius.x = angle_to_radius(angle.x);
    _radius.y = angle_to_radius(angle.y);
    _radius.z = angle_to_radius(angle.z);
}

void GLActionRotateBy::update(float dt)
{
    kmVec3 tt;
    kmVec3 cur = _target->getRotate();
    
    dt = (dt - _lastValue); // 取得增量
    
    tt.x = _radius.x * dt + cur.x;
    tt.y = _radius.y * dt + cur.y;
    tt.z = _radius.z * dt + cur.z;
    
    _target->setRotate(cur);
}

#pragma mark -
#pragma mark GLActionRotateZBy

GLActionRotateZBy::GLActionRotateZBy(GLObject2D * obj ,unsigned long actionTag,float duration,float angle):GLObjectAction(obj,actionTag,duration)
{
    _radius = angle_to_radius(angle);
}

void GLActionRotateZBy::update(float dt)
{
    float tt;
    tt = _radius * (dt - _lastValue);// 取得增量
    float cur = _target->getRotatePtr()->z;
    _target->rotateZ(cur+tt);
}

#pragma mark -
#pragma mark GLActionRotateTo

GLActionRotateTo::GLActionRotateTo(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec3 angle):GLActionRotateBy(obj,actionTag,duration,angle)
{
    _radius.x = angle_to_radius(angle.x);
    _radius.y = angle_to_radius(angle.y);
    _radius.z = angle_to_radius(angle.z);
    
    kmVec3 *  t = obj->getRotatePtr();
    
    //计算当前的rotate 值 和 rotate to的值的差值.
    _radius.x = _radius.x - t->x;
    _radius.y = _radius.y - t->y;
    _radius.z = _radius.z - t->z;
}

#pragma mark -
#pragma mark GLActionRotateZTo

GLActionRotateZTo::GLActionRotateZTo(GLObject2D * obj ,unsigned long actionTag,float duration,float angle):GLActionRotateZBy(obj,actionTag,duration,angle)
{
    _radius = angle_to_radius(angle);
    
    kmVec3 *  t = obj->getRotatePtr();
    //计算当前的rotate Z 值 和 rotate to的Z值的差值.
    _radius = _radius - t->z;
}


#pragma mark -
#pragma mark GLActionBezierMoveTo

// Bezier cubic formula:
//	((1 - t) + t)3 = 1
// Expands to…
//   (1 - t)3 + 3t(1-t)2 + 3t2(1 - t) + t3 = 1
static inline float GLActionBezierMoveBy_bezierat( float a, float b, float c, float d, double t )
{
	return (powf(1-t,3) * a +
			3*t*(powf(1-t,2))*b +
			3*powf(t,2)*(1-t)*c +
			powf(t,3)*d );
}

GLActionBezierMoveBy::GLActionBezierMoveBy(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec2 ctl1,kmVec2 ctl2,kmVec2 end):GLObjectAction(obj,actionTag,duration)
{
    kmVec3 t = obj->getPosition();
    _start_pos.x = t.x;
    _start_pos.y = t.y;
    
    _ctl_p1 = ctl1;
    _ctl_p2 = ctl2;
    _end_p = end;
}


GLActionBezierMoveTo::GLActionBezierMoveTo(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec2 ctl1,kmVec2 ctl2,kmVec2 end) : GLActionBezierMoveBy(obj,actionTag,duration,ZeroVec2,ZeroVec2,ZeroVec2)
{
    
    /*
     这里的 控制点1，2 和结束点   是在实际的坐标系里面的点。
     
     但是实际上，我们在计算点的时候  用的是参考点。
     就是把 target 的坐标点当成（0，0）点来计算的。
     所以这里我们要先做减法来，计算出相对的点。
     */
    
    kmVec3 t = obj->getPosition();
    _start_pos.x = t.x;
    _start_pos.y = t.y;
    
    //_ctl_p1 = ctl1;
    kmVec2Subtract(&_ctl_p1, &ctl1, &_start_pos);
    //_ctl_p2 = ctl2;
    kmVec2Subtract(&_ctl_p2, &ctl2, &_start_pos);
    //_end_p = end;
    kmVec2Subtract(&_end_p, &end, &_start_pos);
}


//TODO: implemtns && modify
void GLActionBezierMoveBy::update(float dt)
{
    float _curTime = dt;
    
    // scale x,y to [0-1]
    // the reveser to [wxh]
    // update.
	float xa = 0;
	float xb = _ctl_p1.x;
	float xc = _ctl_p2.x;
	float xd = _end_p.x;
    
	float ya = 0;
	float yb = _ctl_p1.y;
	float yc = _ctl_p2.y;
	float yd = _end_p.y;
    
	float x = GLActionBezierMoveBy_bezierat(xa, xb, xc, xd, _curTime);
	float y = GLActionBezierMoveBy_bezierat(ya, yb, yc, yd, _curTime);
    
    _target->setPosition(_start_pos.x + x , _start_pos.y + y);
}

#pragma mark -
#pragma mark GLActionMoveBy
GLActionMoveBy::GLActionMoveBy(GLObject2D * obj ,unsigned long actionTag,float duration,float x,float y, float z):GLObjectAction(obj,actionTag,duration)
{
    _pos.x = x;
    _pos.y = y;
    _pos.z = z;
}

GLActionMoveBy::GLActionMoveBy(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec3 position):GLObjectAction(obj,actionTag,duration)
{
    //算他们的差值
    _pos.x = position.x;
    _pos.y = position.y;
    _pos.z = position.z;
}

// dt 增量， 在这里时间片里面要增加的量
void GLActionMoveBy::update(float dt)
{
    dt = (dt - _lastValue);// 取得增量
    kmVec3 t = _target->getPosition();
    _target->setPosition(_pos.x * dt + t.x,  _pos.y * dt + t.y,  _pos.z * dt + t.z);
}

#pragma mark -
#pragma mark GLActionMoveTo
GLActionMoveTo::GLActionMoveTo(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec3 position):GLActionMoveBy(obj,actionTag,duration,position)
{
    //算他们的差值
    kmVec3 t = _target->getPosition();
    _pos.x = position.x - t.x;
    _pos.y = position.y - t.y;
    _pos.z = position.z - t.z;
}

GLActionMoveTo::GLActionMoveTo(GLObject2D * obj ,unsigned long actionTag,float duration,float x,float y,float z):GLActionMoveBy(obj,actionTag,duration,x,y,z)
{
    kmVec3 t = _target->getPosition();
    _pos.x = x - t.x;
    _pos.y = y - t.y;
    _pos.z = z - t.z;
}



#pragma mark -
#pragma mark GLActionFadeTo

GLActionFadeTo::GLActionFadeTo(GLObject2D * obj ,unsigned long actionTag,float duration, float alpha):GLObjectAction(obj,actionTag,duration)
{
    _alpha = alpha - obj->getAlphaValue();
};

void GLActionFadeTo::update(float dt)
{
    _target->setAlphaValue(_target->getAlphaValue() + _alpha * (dt - _lastValue));
}


#pragma mark -
#pragma mark GLActionFadeIn
void GLActionFadeIn::update(float dt)
{
    _target->setAlphaValue(dt);
}

#pragma mark -
#pragma mark GLActionFadeOut
void GLActionFadeOut::update(float dt)
{
    _target->setAlphaValue(1.0 - dt);
}


#pragma mark -
#pragma mark GLActionScaleTo
GLActionScaleTo::GLActionScaleTo(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec2 scale):GLObjectAction(obj,actionTag,duration)
{
    kmVec3 t = _target->getScale();
    _scale.x = scale.x - t.x;
    _scale.y = scale.x - t.y;
}


GLActionScaleTo::GLActionScaleTo(GLObject2D * obj ,unsigned long actionTag,float duration,float x,float y):GLObjectAction(obj,actionTag,duration)
{
    kmVec3 t = _target->getScale();
    _scale.x = x - t.x;
    _scale.y = y - t.y;
}

void GLActionScaleTo::update(float dt)
{
    dt = (dt - _lastValue);//取得增量
    kmVec3 t = _target->getScale();
    _target->setScale(_scale.x * dt + t.x, _scale.y * dt + t.y, t.z);
}


#pragma mark -
#pragma mark GLActionScaleBy
GLActionScaleBy::GLActionScaleBy(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec2 scale):GLObjectAction(obj,actionTag,duration)
{
    _scale.x = scale.x;
    _scale.y = scale.x;
}

GLActionScaleBy::GLActionScaleBy(GLObject2D * obj ,unsigned long actionTag,float duration,float x,float y):GLObjectAction(obj,actionTag,duration)
{
    _scale.x = x;
    _scale.y = y;
}

void GLActionScaleBy::update(float dt)
{
    kmVec3 t = _target->getScale();
    _target->setScale(_scale.x * dt + t.x, _scale.y * dt + t.y, t.z);
}

#pragma mark -
#pragma mark GLActionDelay
GLActionDelay::~GLActionDelay()
{
    if (_action)
        delete _action;
    LOG("release delay action");
}

GLActionDelay::GLActionDelay(GLObjectAction * action, unsigned long actTag, float delay):GLObjectAction(nullptr,actTag,(action->duration()+delay))
{
    GLAssert(action,"invalied argments action can not be NULL!!!");
    _delay = delay;
    _action = action;
}

void GLActionDelay::stepIn(float dt)
{
    _curTime+=dt;
    if (_curTime < _delay)
    {
        return;
    }
    
    _action->stepIn(dt);
    _stop = _action->isFinished();
}

void GLActionDelay::reset2begin()
{
    GLObjectAction::reset2begin();
    _action->reset2begin();
}


#pragma mark -
#pragma mark GLActionRepeats

GLActionRepeats::~GLActionRepeats()
{
    if (_action)
        delete _action;
    LOG("release GLActionRepeatForever action");
    GLObjectAction::~GLObjectAction();
}

GLActionRepeats::GLActionRepeats(GLObjectAction * action, unsigned long actTag, unsigned int repeat):GLObjectAction(nullptr,actTag,INT_MAX)
{
    GLAssert(action, "action can not be NULL!!!");
    _action = action;
    _repeats = repeat;
}

void GLActionRepeats::stepIn(float dt)
{
    _action->stepIn(dt);
    
    if(_action->isFinished())
    {
        _cur++;
        _action->reset2begin();
    }
    
    if (_cur == _repeats)
    {
        _stop = true;
    }
}

void GLActionRepeats::reset2begin()
{
    GLObjectAction::reset2begin();
    _cur = 0;
    _action->reset2begin();
}


#pragma mark -
#pragma mark GLActionRepeatForever

GLActionRepeatForever::~GLActionRepeatForever()
{
    GLActionRepeats::~GLActionRepeats();
};
GLActionRepeatForever::GLActionRepeatForever(GLObjectAction * action, unsigned long actTag):GLActionRepeats(action,actTag, INT_MAX){}

#pragma mark -
#pragma mark GLActionSpawn

GLActionSpawn::~GLActionSpawn()
{
    LOG("spawn release sub actions");
    for (std::vector<GLObjectAction *>::iterator it = _subActions.begin(); it != _subActions.end(); it++)
    {
        delete (*it);
    }
    _subActions.clear();
}

void GLActionSpawn::stepIn(float dt)
{
    bool flag = true;
    for (std::vector<GLObjectAction *>::iterator it = _subActions.begin(); it != _subActions.end(); it++)
    {
        GLObjectAction * action = (*it);
        if(!action->isFinished())
        {
            action->stepIn(dt);
            flag = false;
        }
    }
    _stop = flag;
}

void GLActionSpawn::reset2begin()
{
    GLObjectAction::reset2begin();
    for (std::vector<GLObjectAction *>::iterator it = _subActions.begin(); it != _subActions.end(); it++)
    {
        (*it)->reset2begin();
    }
}


#pragma mark -
#pragma mark GLActionSequence

GLActionSequence::~GLActionSequence()
{
    for (std::vector<GLObjectAction *>::iterator it = _subActions.begin(); it != _subActions.end(); it++)
    {
        delete (*it);
    }
    _subActions.clear();
}


void GLActionSequence::stepIn(float dt)
{
    // check the sequenece
    // if is already done.
    std::vector<GLObjectAction *>::size_type size = _subActions.size();
    if (_curIndex == size) // not size-1, and the blew when finished will ++.
    {
        _stop = true;
        return;
    }
    
    // done
    GLObjectAction * action = _subActions[_curIndex];
    action->stepIn(dt);
    
    
    // check the current action if is finished running.
    if(action->isFinished())
        _curIndex++;
}


void GLActionSequence::reset2begin()
{
    GLObjectAction::reset2begin();
    _curIndex = 0;
    
    for (std::vector<GLObjectAction *>::iterator it = _subActions.begin(); it != _subActions.end(); it++)
    {
        (*it)->reset2begin();
    }
}


////////////////////////////////////////////////
////////////////////////////////////////////////

#pragma mark -
#pragma mark this is for animition
#include "GLTexture.h"

GLAnimation::~GLAnimation()
{
    LOG("GLAnimation release, will remove texture from manager.");
    
    if (_autoCleanFrames)
    {
        vector<GLTexture *>::size_type size = _animationFrame.size();
        for (int idx = 0; idx < size ; idx++)
        {
            GLTextureManager::manager()->removeCacheTexture(_animationFrame[idx]);
        }
    }
}

GLAnimation::GLAnimation()
{
    _autoCleanFrames = true;
}

GLAnimation::GLAnimation(bool autoClean)
{
    _autoCleanFrames = autoClean;
}


GLTexture * GLAnimation::frameAtIndex(int idx)
{
    vector<GLTexture *>::size_type size = _animationFrame.size();
    if (idx >= size || idx < 0)
    {
        return nullptr;
    }
    return _animationFrame[idx];
}


void GLAnimation::addFrame(GLTexture * frame)
{
    _animationFrame.push_back(frame);
}


void GLAnimation::setDuration(float duration)
{
    vector<GLTexture *>::size_type size = _animationFrame.size();
    _perframeDelay = duration / ( size * 1.0 );
}

void GLAnimation::setFrameDelay(float delay)
{
    _perframeDelay = delay;
}


GLAnimate::~GLAnimate()
{
    delete _objectAnimation;
}

GLAnimate::GLAnimate(GLObject2D * obj,GLAnimation * animation, unsigned long actTag):GLObjectAction(obj, actTag,0)
{
    GLAssert(animation, "animation for object can not for NULL");
    
    _objectAnimation = animation;
    _duration = animation->frameDelay() * animation->frameCount();
    _objectTexture = _target->getTexture();
}


void GLAnimate::stepIn(float dt)
{
    if (_curTime > _duration - 0.0002)
    {
        _target->setTexture(_objectTexture);
        _stop = true;
        return;
    }
    
    // get the current index
    int curIndex = static_cast<int>(floor(_curTime / _objectAnimation->frameDelay()));
    GLTexture * curTex = _objectAnimation->frameAtIndex(curIndex);
    
    // update texture.(target will auto check if need to update texture)
    _target->setTexture(curTex);
    
    // rasie the time.
    _curTime += dt;
}

#pragma mark -
#pragma mark action manager

void GLObjectActionManager::addAction(GLObjectAction *action)
{
    _actionList.push_back(action);
}

void GLObjectActionManager::removeAction(GLObjectAction * action, bool clean)
{
    for (list<GLObjectAction*>::iterator it= _actionList.begin(); it != _actionList.end(); ++it)
    {
        if ((*it) == action)
        {
            if(clean)
            {
                delete (*it);
            }
            _actionList.erase(it);
            
            // aleady found.
            // break loop
            break;
        }
    }
}

void GLObjectActionManager::removeActionByTag(unsigned long tag, bool clean)
{
    for (list<GLObjectAction*>::iterator it= _actionList.begin(); it != _actionList.end(); ++it)
    {
        if ((*it)->_actionTag == tag)
        {
            if(clean)
            {
                delete (*it);
            }
            _actionList.erase(it);
            
            // aleady found.
            // break loop
            break;
        }
    }
}

void GLObjectActionManager::removeAllAction()
{
    for (list<GLObjectAction*>::iterator it= _actionList.begin(); it != _actionList.end(); ++it)
    {
        delete (*it);
    }
    _actionList.clear();
}


void GLObjectActionManager::setActionDoneCallBack(function<bool (GLObjectAction *)>  callback)
{
    _actionCallBack = callback;
}

void GLObjectActionManager::stepIn(float dt)
{
    for (list<GLObjectAction*>::iterator it = _actionList.begin(); it != _actionList.end(); ++it)
    {
        if ((*it)->isFinished())
        {
            if (_actionCallBack)
            {
                // tell callback it was done
                // and ask it if need to be clean
                bool ret = _actionCallBack((*it));
                if(ret)
                    delete (*it);
                _actionList.erase(it);
            }
            else
            {
                // if have no call back
                // remove it derectly
                delete (*it);
                _actionList.erase(it);
            }// end callback if
        }
        else
        {
            (*it)->stepIn(dt);
        }// end finished if
        
    }// end for
}
