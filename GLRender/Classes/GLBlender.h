//
//  GLBlender.h
//  GLRender
//
//  Created by dinosaur on 13-12-3.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef GLRender_GLBlender_h
#define GLRender_GLBlender_h

#include "GLBase.h"

class GLBlender : public GLBase
{
public:
    
    DECLARE_CLASS(GLBlender)
    
	//! source blend function
	unsigned int src = GL_ONE;
	//! destination blend function
	unsigned int dst = GL_ONE_MINUS_SRC_ALPHA;
    
    GLBlender() = default;
    ~GLBlender() = default;
    GLBlender(unsigned int s, unsigned int d)
    {
        src = s; dst = d;
    }
    
    void makeDefault()
    {
        src = GL_ONE;
        dst = GL_ONE_MINUS_SRC_ALPHA;
    }
    
    bool isDefualt()
    {
        return (src == GL_ONE && dst == GL_ONE_MINUS_SRC_ALPHA);
    }
    
    GLBlender & operator=(GLBlender & b)
    {
        this->src = b.src;
        this->dst = b.dst;
        return (*this);
    }
    
    bool operator==(GLBlender & b)
    {
        return (this->src == b.src && this->dst == b.dst);
    }
    
    void blend();
private:
    static unsigned int previousSrc;
    static unsigned int previousDst;
};

#endif
