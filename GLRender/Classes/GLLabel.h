//
//  GLLabel.h
//  GLRender
//
//  Created by dinosaur on 13-12-19.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

#ifndef __GLRender__GLLabel__
#define __GLRender__GLLabel__

#include "GLObject.h"
#include "Utilities.h"

/*
 lable 可以显示文字。
 关于文字的颜色的原理是这样的。
 纹理是白色的，文字颜色给定，在GPU里面 通过color * texColor 来显示。
 这样做的好处:
    动态改变文字color效率很好的。
    因为文字纹理图片不需要重新绘制。
 
 当然，这个要根据后续的测试来搞看性能怎么样。
 
 ====================
 还有就是不要再label 里面update texture。
 要改变字   就一直create 新的就OK。
 不用了  删除就OK了。
 */


/*
 *  多行标签。
 *  需要给定一个 size的大小。
 *  然后文字(结合属性)会绘制在这个 size大小的纹理上面。
 *  目前没有实现 垂直方面的那个参数。
 *  只实现了 水平和换行的那个参数。
 */

class GLMutableLineLabel : public GLSprite
{
public:
    
    DECLARE_CLASS(GLMutableLineLabel);
    
    GLMutableLineLabel():GLSprite(){};
    ~GLMutableLineLabel();
    bool init(string & text,
                       string & fontName,
                       float fontSize,
                       Color4B color,
                       GLHTextAlignment halign,
                       GLVTextAlignment valign,
                       GLLineBreakMode breakMode,
                       float width,
                       float height);
    
    
    bool init(string & text,
                       string & fontName,
                       float fontSize,
                       Color4B color,
                       float width,
                       float height);
    
    
    
    
    bool init(const char * text,
                       const char * fontName,
                       float fontSize,
                       Color4B color,
                       GLHTextAlignment halign,
                       GLVTextAlignment valign,
                       GLLineBreakMode breakMode,
                       float width,
                       float height);
    
    
    bool init(const char * text,
                       const char * fontName,
                       float fontSize,
                       Color4B color,
                       float width,
                       float height);
    
    virtual void visit();
    virtual void usePorgram();
    
private:
    float       _textSize;
    Color4B     _color;
    
    GLHTextAlignment    _h_align;
    GLVTextAlignment    _v_align;
    GLLineBreakMode     _break_mode;
};

/*
 *  单行标签，需要的纹理的大小是自动计算的。
 */
class GLLabel : public GLMutableLineLabel
{
public:
    
    DECLARE_CLASS(GLLabel);
    GLLabel():GLMutableLineLabel(){};
    ~GLLabel();
    
    bool init(string & text,string & fontName,float fontSize, Color4B color,GLHTextAlignment halign,GLVTextAlignment valign,GLLineBreakMode breakMode);
    bool init(string & text,string & fontName,float fontSize, Color4B color);
    
    
    bool init(const char *text,const char *fontName,float fontSize, Color4B color,GLHTextAlignment halign,GLVTextAlignment valign,GLLineBreakMode breakMode);
    bool init(const char *text,const char *fontName,float fontSize, Color4B color);
};



#endif /* defined(__GLRender__GLLabel__) */
