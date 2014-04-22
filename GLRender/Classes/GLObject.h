//
//  GLObject.h
//  GLRender
//
//  Created by dinosaur on 13-11-28.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef __GLRender__GLObject__
#define __GLRender__GLObject__



#ifdef __cplusplus
extern "C" {
#endif
    // color [0-255]
    struct Color4B
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };
    
    static inline Color4B
    MakeColor4B(const unsigned char r,
                const unsigned char g,
                const unsigned char b,
                const unsigned char o)
    {
        Color4B c = {r, g, b, o};
        return c;
    }
    
    struct PerPointInfo
    {
        float  vertices[2]; // vertices
        float  texCoords[2]; // texture coords.
        struct Color4B color;
    };
    
    
    // l -- left
    // r -- right
    // d -- down
    // u -- up
    //以左下角为起点，顺时针
    
    /* for draw array
     3      4
     
     1      2
     */
    struct Quad
    {
        PerPointInfo ld;
        PerPointInfo rd;
        PerPointInfo lu;
        PerPointInfo ru;
    };
    
    
#ifdef __cplusplus
}
#endif

#include "GLObjectBase.h"


using namespace std;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


/*
 左下角是（0，0）点
 */
class GLTexture;
class GLObjectContainer;

class GLObject2D : public GLObjectBase
{
public:
    
    DECLARE_CLASS(GLObject2D)
    
    GLObject2D() : GLObjectBase(){};
    GLObject2D(const kmVec2 size,const kmVec2 pos);
    GLObject2D(float posX, float posY,float w, float h);
    GLObject2D(Color4B start, Color4B end);
    virtual ~GLObject2D();
    
    
    // public vars
    void rotateZ(float angle);
    
    //overwrite
    virtual void visit();
    virtual void draw();
    virtual void usePorgram();
    
    // 2d size.
    void setSize(float w, float h);
    void setSize(kmVec2 sv);
    kmVec2 size();
    
    
    void setTexture(GLTexture * tex);
    GLTexture * getTexture();
    
    // set color
    void setColor(Color4B start, Color4B end);
    void setColor(Color4B color);
    void setStartColor(Color4B color);
    void setEndColor(Color4B color);
    
protected:
    GLBlender               _blend;
    Quad                    _quadInfo = {0};
    GLTexture               *_texture = nullptr;
    
    void initPosition();
    void initTexCoord();
    void transform();
    
    friend GLObjectContainer;
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// container only draw child. not draw itself.
class GLObjectContainer : public GLObject2D
{
public:
    
    DECLARE_CLASS(GLObjectContainer)
    
    virtual void visit();
    virtual void draw();
    virtual void usePorgram();
    
    GLObjectContainer() : GLObject2D() {};
    GLObjectContainer(const kmVec2 size,const kmVec2 pos):GLObject2D(size,pos){};
    GLObjectContainer(float posX, float posY,float w, float h):GLObject2D(posX,posY,w,h){};
    GLObjectContainer(Color4B start, Color4B end):GLObject2D(start,end){};
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// sprite will draw it self first
// then draw child.
// not draw color .
class GLSprite : public GLObjectContainer
{
public:
    
    DECLARE_CLASS(GLSprite)
    
    virtual void visit();
    virtual void draw();
    virtual void usePorgram();
    
    GLSprite() : GLObjectContainer() {};
    GLSprite(const kmVec2 size,const kmVec2 pos):GLObjectContainer(size,pos){};
    GLSprite(float w, float h, float posX, float posY):GLObjectContainer(w,h,posX,posY){};
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// GLColorLayer will draw it self first
// then draw child.
// only draw the color
class GLColorLayer : public GLObjectContainer
{
public:
    
    DECLARE_CLASS(GLColorLayer)
    
    GLColorLayer() : GLObjectContainer() {};
    GLColorLayer(const kmVec2 size,const kmVec2 pos):GLObjectContainer(size,pos){};
    GLColorLayer(float w, float h, float posX, float posY):GLObjectContainer(w,h,posX,posY){};
    GLColorLayer(Color4B start, Color4B end):GLObjectContainer(start,end){};
    
    virtual void visit();
    virtual void draw();
    virtual void usePorgram();
};


#endif /* defined(__GLRender__GLObject__) */
