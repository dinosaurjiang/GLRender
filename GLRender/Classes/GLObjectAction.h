//
//  GLObjectAction.h
//  GLRender
//
//  Created by dinosaur on 13-12-10.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef __GLRender__GLObjectAction__
#define __GLRender__GLObjectAction__

#include <iostream>
#include <list>
#include "GLTransitions.h"
#include "vec3.h"
#include "vec2.h"

class GLObject2D;
class GLObjectActionManager;
class GLActionRepeatForever;


using namespace std;

#pragma mark -
////////////////////////////////////////////////
/////  base class of action.
////////////////////////////////////////////////
class GLObjectAction
{
    
public:
    
    GLObjectAction(GLObject2D * obj ,unsigned long actionTag, float duration);
    virtual ~GLObjectAction() = default;
    
    virtual void update(float dt);
    virtual void stepIn(float dt);
    virtual bool isFinished();
    virtual void reset2begin();
    
    unsigned long Tag();
    void stop();
    float duration();
    GLObject2D * target();
    
    TransiltionTimingFunction   actionTimingFunction = kTransiltionTimingFunctionLinear;
protected:
    
    GLObject2D *    _target = nullptr;
    float           _duration = 1.0;
    float           _curTime = 0.0;
    bool            _stop = false;
    unsigned long   _actionTag = -1;
    
    float           _lastValue = 0.0f;
    /*
     * those friends need to access private & protected vars.
     */
    friend GLObjectActionManager;
};


#pragma mark -


typedef void (*GLActionCallback_Function)(void * ctx);
class GLActionCallback : public GLObjectAction
{
public:
    GLActionCallback(unsigned long actionTag, GLActionCallback_Function func, void * ctx);
    virtual void stepIn(float dt);
private:
    GLActionCallback_Function _callback_func;
    void * _callback_ctx;
};


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


class GLActionRotateBy : public GLObjectAction
{
public:
    virtual ~GLActionRotateBy() = default;
    GLActionRotateBy(GLObject2D * obj ,
                     unsigned long actionTag,
                     float duration,
                     kmVec3 angle);
    
    virtual void update(float dt);
    
protected:
    kmVec3          _radius = {0};

};


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


class GLActionRotateZBy : public GLObjectAction
{
public:
    virtual ~GLActionRotateZBy() = default;
    GLActionRotateZBy(GLObject2D * obj ,
                     unsigned long actionTag,
                     float duration,
                     float angle);
    
    virtual void update(float dt);
    
protected:
    float       _radius = 0.0f;
};


///////////////////////////////////////////////////////
///  just rotate in 2D.
///////////////////////////////////////////////////////
class GLActionRotateZTo : public GLActionRotateZBy
{
public:
    virtual ~GLActionRotateZTo() = default;
    GLActionRotateZTo(GLObject2D * obj ,
                      unsigned long actionTag,
                      float duration,
                      float angle);
};

///////////////////////////////////////////////////////
//  rotate in 3D
///////////////////////////////////////////////////////


class GLActionRotateTo : public GLActionRotateBy
{
public:
    virtual ~GLActionRotateTo() = default;
    GLActionRotateTo(GLObject2D * obj ,
                     unsigned long actionTag,
                     float duration,
                     kmVec3 angle);
};


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


class GLActionMoveBy : public GLObjectAction
{
public:
    virtual ~GLActionMoveBy() = default;
    GLActionMoveBy(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec3 position);
    GLActionMoveBy(GLObject2D * obj ,unsigned long actionTag,float duration,float x,float y,float z);
    virtual void update(float dt);
    
protected:
    kmVec3       _pos = {0.0f};
};

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


class GLActionMoveTo : public GLActionMoveBy
{
public:
    virtual ~GLActionMoveTo() = default;
    GLActionMoveTo(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec3 position);
    GLActionMoveTo(GLObject2D * obj ,unsigned long actionTag,float duration,float x,float y,float z);
};



///////////////////////////////////////////////////////
///////////////////////////////////////////////////////



class GLActionBezierMoveBy : public GLObjectAction
{
public:
    virtual ~GLActionBezierMoveBy() = default;
    GLActionBezierMoveBy(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec2 ctl1,kmVec2 ctl2,kmVec2 end);
    virtual void update(float dt);
    
protected:
    kmVec2       _start_pos = {0.0f};
    kmVec2       _ctl_p1 = {0.0f};
    kmVec2       _ctl_p2 = {0.0f};
    kmVec2       _end_p = {0.0f};
};

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

class GLActionBezierMoveTo : public GLActionBezierMoveBy
{
public:
    GLActionBezierMoveTo(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec2 ctl1,kmVec2 ctl2,kmVec2 end);
};



///////////////////////////////////////////////////////
///////////////////////////////////////////////////////



class GLActionFadeTo : public GLObjectAction
{
public:
    GLActionFadeTo(GLObject2D * obj ,unsigned long actionTag,float duration, float alpha);
    virtual void update(float dt);
private:
    float _alpha;
};


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


class GLActionFadeIn : public GLObjectAction
{
public:
    GLActionFadeIn(GLObject2D * obj ,unsigned long actionTag,float duration):GLObjectAction(obj,actionTag,duration){};
    virtual void update(float dt);
};


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


class GLActionFadeOut : public GLObjectAction
{
public:
    GLActionFadeOut(GLObject2D * obj ,unsigned long actionTag,float duration):GLObjectAction(obj,actionTag,duration){};
    virtual void update(float dt);
};


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


class GLActionScaleTo : public GLObjectAction
{
public:
    virtual ~GLActionScaleTo() = default;
    GLActionScaleTo(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec2 scale);
    GLActionScaleTo(GLObject2D * obj ,unsigned long actionTag,float duration,float x,float y);
    virtual void update(float dt);
private:
    kmVec2       _scale = {0.0f};
};



///////////////////////////////////////////////////////
///////////////////////////////////////////////////////



class GLActionScaleBy : public GLObjectAction
{
public:
    virtual ~GLActionScaleBy() = default;
    GLActionScaleBy(GLObject2D * obj ,unsigned long actionTag,float duration,kmVec2 scale);
    GLActionScaleBy(GLObject2D * obj ,unsigned long actionTag,float duration,float x,float y);
    virtual void update(float dt);
private:
    kmVec2       _scale = {0.0f};
};



///////////////////////////////////////////////////////
///////////////////////////////////////////////////////



class GLActionDelay : public GLObjectAction
{
public:
    virtual ~GLActionDelay();
    GLActionDelay(GLObjectAction * action, unsigned long actTag, float delay);
    virtual void stepIn(float dt);
    virtual void reset2begin();
private:
    float               _delay;
    GLObjectAction*     _action;
};



////////////////////////////////////////////////
////////////////////////////////////////////////



class GLActionRepeats : public GLObjectAction
{
public:
    virtual ~GLActionRepeats();
    GLActionRepeats(GLObjectAction * action, unsigned long actTag, unsigned int repeat);
    virtual void stepIn(float dt);
    virtual void reset2begin();
private:
    unsigned int        _cur = 0;
    unsigned int        _repeats = 0;
    GLObjectAction*     _action;
};


////////////////////////////////////////////////
////////////////////////////////////////////////


class GLActionRepeatForever : public GLActionRepeats
{
public:
    virtual ~GLActionRepeatForever();
    GLActionRepeatForever(GLObjectAction * action, unsigned long actTag);
};

////////////////////////////////////////////////
////////////////////////////////////////////////


#include <vector>
class GLActionSpawn : public GLObjectAction
{
public:
    virtual ~GLActionSpawn();
    GLActionSpawn(unsigned long tag):GLObjectAction(nullptr, tag, 0){};
    virtual void stepIn(float dt);
    virtual void reset2begin();
    void addAction(GLObjectAction * obj)
    {
        _subActions.push_back(obj);
    }
private:
    std::vector<GLObjectAction *> _subActions;
};



////////////////////////////////////////////////
////////////////////////////////////////////////



class GLActionSequence : public GLObjectAction
{
public:
    
    virtual ~GLActionSequence();
    GLActionSequence(unsigned long tag):GLObjectAction(nullptr, tag, 0){};
    virtual void stepIn(float dt);
    virtual void reset2begin();
    void addAction(GLObjectAction * obj)
    {
        _subActions.push_back(obj);
    }
    
private:
    unsigned int                    _curIndex = 0;
    std::vector<GLObjectAction *>   _subActions;
};



////////////////////////////////////////////////
////////////////////////////////////////////////


#pragma mark -
#pragma mark this is for animition
#include <vector>

class GLTexture;
class GLAnimation
{
public:
    ~GLAnimation();
    GLAnimation();
    GLAnimation(bool autoClean);
    
    void addFrame(GLTexture * frame);
    void setDuration(float duration);
    void setFrameDelay(float delay);
    
    GLTexture * frameAtIndex(int idx);
    float frameDelay(){return _perframeDelay; }
    unsigned long frameCount(){return _animationFrame.size();}
private:
    bool                    _autoCleanFrames;
    float                   _perframeDelay;
    vector<GLTexture *>     _animationFrame;
};


////////////////////////////////////////////////
////////////////////////////////////////////////


class GLAnimate : public GLObjectAction
{
public:
    virtual ~GLAnimate();
    GLAnimate(GLObject2D * obj,GLAnimation * animation, unsigned long actTag);
    virtual void stepIn(float dt);
private:
    GLTexture*          _objectTexture;
    GLAnimation*        _objectAnimation = nullptr;
};

#pragma mark -
#pragma mark GLObjectActionManager
////////////////////////////////////////////////
/*
 *  这是我自己用的action manager
 *  具体来说，一个object  只能有一个action 比较好。
 *  当然也可以有多个。但是如果有多个对于同一个属性的修改的话，会出错的。
 *  比如说有两个move to action。 如果两个action 都update 了object 的坐标。
 *  就不知道会是什么样子的了。
 ********************
 *  这个类管理了所有的action，action 完成的时候，会自动释放的。 理论上全部用他自动清理比较好
 *
 */
class GLObjectActionManager
{
public:
    
    static GLObjectActionManager * manager()
    {
        static GLObjectActionManager _obj_action_manager;
        return &_obj_action_manager;
    }
    // action which added to list.
    // will auto delete by manager if it was finished.
    void addAction(GLObjectAction *action);
    
    // stop the action and remove it from list.
    // manager will auto delete it and free memory.
    void removeAction(GLObjectAction * action, bool clean = true);
    void removeActionByTag(unsigned long tag, bool clean = true);
    void removeAllAction();
    
    //dt:时间间隔
    void stepIn(float dt);
    void setActionDoneCallBack(function<bool (GLObjectAction *)>  callback);
    
private:
    GLObjectActionManager() = default;
    ~GLObjectActionManager() = default;
    
    function<bool (GLObjectAction *)> _actionCallBack;
    list<GLObjectAction *> _actionList;
};


#endif /* defined(__GLRender__GLObjectAction__) */
