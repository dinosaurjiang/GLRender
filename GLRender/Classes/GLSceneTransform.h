//
//  GLSceneTransform.h
//  GLRender
//
//  Created by dinosaur on 13-12-26.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

#ifndef __GLRender__GLSceneTransform__
#define __GLRender__GLSceneTransform__

#include "GLObject.h"
#include "GLTransitions.h"

class GLTexture;
class GLScene;

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     *  这里的四边形的draw  都是按照gl_triangle 来的。没有用strip 方式
     *  虽然说strip 快，但是有时候会遇到很多三角形的时候。strip 就不合适了。
     *  如果用gldrawelements南。感觉这里也不会有太多的三角形，
     *  就是一般的变换。也不用那么麻烦。也大多是些规则的图像
     *  其实里面用的是三角形，也可以拼凑些不规则图像来的。
     ********************************************************
     *  下面的struct 是transform 专用的。估计其它地方也用不到
     ********************************************************
     *  point info
     *  4   3
     *  1   2
     *  draw order is: 1,2,4,   2,4,3
     */
    
    typedef enum Orientation {
        Ori_Unknow,
        Ori_Left,
        Ori_Right,
        Ori_Up,
        Ori_Down
    }Orientation;
    

    typedef struct Transform3DPoint
    {
        kmVec3 XYZ;// coord
        kmVec2 UV;// texture
    }Transform3DPoint;
    
    
    typedef struct Transform3DQuad
    {
        //第一个三角形的数据
        Transform3DPoint t1p1;
        Transform3DPoint t1p2;
        Transform3DPoint t1p3;
        
        Transform3DPoint t2p1;
        Transform3DPoint t2p2;
        Transform3DPoint t2p3;
        
    }Transform3DQuad;
#ifdef __cplusplus
}
#endif

/*
 这个是一个scene 到另外一个scene 中间的过渡效果
 这个类仅仅是基类。
 主要就是把一个图换到另外一个个图上面去。
 
 这个是top-level-obj 不能被 add as child。
 */

class GLSceneTransform : public GLObjectBase
{
public:
    
    DECLARE_CLASS(GLSceneTransform)
    
    ~GLSceneTransform();
    GLSceneTransform(GLScene * from, GLScene * to, int fquadCnt,int tquadCnt, bool fromFirst);
    
    
    // overwrite...
    void visit();
    void draw();
    void usePorgram();
    void update(float dt);
    
    
    // if is update.
    // please do not set this.
    void setDuration(float v)
    {
        _duration = v;
    }
    
    float getDuration()
    {
        return _duration;
    }
    
    bool isStop()
    {
        return _isStop;
    }
    
    
protected:
    
    void getReady();
    void initTextures();
    
    /*
     * sub class only need to overwrite those method is OK.
     */
    virtual void updateCoordinate(float dt);
    
    GLBlender       _blend;
    
    GLScene*        _fromScene;
    GLScene*        _toScene;
    
    /*
     *  those texure will be delete affter using.
     */
    
    GLTexture*                  _fromTexture;
    Transform3DQuad*            _fromQuadList;
    int                         _fromQuadCount;
    float                       _fromAlpha;
    
    
    GLTexture*                  _toTexture;
    int                         _toQuadCount;
    Transform3DQuad*            _toQuadList;
    float                       _toAlpha;
    
    
    TransiltionTimingFunction   timingFunction;
    
    float       _lastValueFlag = 0.0;
    float       _duration = 4.0;
    
    bool        _fromFirstDraw = true;
    
private:
    
    float       _curTime = 0.0;
    bool        _isReady = false;
    bool        _isStop = false;
    
    void drawFromTexture();
    void drawToTexture();
};


////////////////////////////////////////////////////////////////////////////////////
class GLTransformAppear : public GLSceneTransform
{
    
public:
    DECLARE_CLASS(GLTransformAppear)
    
    ~GLTransformAppear() = default;
    GLTransformAppear(GLScene * from, GLScene * to,Orientation ori);
    
protected:
    virtual void updateCoordinate(float dt);
    
    float           _offset;
    Orientation     _ori;
private:
    bool _inited = false;
    void _initTexCoordinateAndOther();
};


////////////////////////////////////////////////////////////////////////////////////
class GLTransformAppearFromFadeOut : public GLTransformAppear
{
public:
    DECLARE_CLASS(GLTransformAppearFromFadeOut)
    ~GLTransformAppearFromFadeOut() = default;
    GLTransformAppearFromFadeOut(GLScene * from, GLScene * to):GLTransformAppear(from,to,Ori_Unknow){_fromFirstDraw = false;};
protected:
    virtual void updateCoordinate(float dt);
};


////////////////////////////////////////////////////////////////////////////////////
class GLTransformAppearToFadeIn : public GLTransformAppear
{
public:
    DECLARE_CLASS(GLTransformAppearToFadeIn);
    ~GLTransformAppearToFadeIn() = default;
    GLTransformAppearToFadeIn(GLScene * from, GLScene * to):GLTransformAppear(from,to,Ori_Unknow){_fromFirstDraw = true;_toAlpha = .0;};
protected:
    virtual void updateCoordinate(float dt);
};


////////////////////////////////////////////////////////////////////////////////////
class GLTransformAppearFromLeft : public GLTransformAppear
{
public:
    DECLARE_CLASS(GLTransformAppearFromLeft)
    ~GLTransformAppearFromLeft() = default;
    GLTransformAppearFromLeft(GLScene * from, GLScene * to):GLTransformAppear(from,to,Ori_Left){};
};


////////////////////////////////////////////////////////////////////////////////////
class GLTransformAppearFromRight : public GLTransformAppear
{
public:
    DECLARE_CLASS(GLTransformAppearFromRight)
    ~GLTransformAppearFromRight() = default;
    GLTransformAppearFromRight(GLScene * from, GLScene * to):GLTransformAppear(from,to,Ori_Right){};
};


////////////////////////////////////////////////////////////////////////////////////
class GLTransformAppearFromTop : public GLTransformAppear
{
public:
    DECLARE_CLASS(GLTransformAppearFromTop)
    ~GLTransformAppearFromTop() = default;
    GLTransformAppearFromTop(GLScene * from, GLScene * to):GLTransformAppear(from,to,Ori_Up){};
};


////////////////////////////////////////////////////////////////////////////////////
class GLTransformAppearFromBottom : public GLTransformAppear
{
public:
    DECLARE_CLASS(GLTransformAppearFromBottom)
    ~GLTransformAppearFromBottom() = default;
    GLTransformAppearFromBottom(GLScene * from, GLScene * to):GLTransformAppear(from,to,Ori_Down){};
};



////////////////////////////////////////////////////////////////////////////////////
class GLTransformDisappear : public GLTransformAppear
{
    DECLARE_CLASS(GLTransformDisappear)
    ~GLTransformDisappear() = default;
    GLTransformDisappear(GLScene * from, GLScene * to,Orientation ori);
protected:
    virtual void updateCoordinate(float dt);
private:
    Orientation _ori_flag;
};

#endif /* defined(__GLRender__GLSceneTransform__) */
