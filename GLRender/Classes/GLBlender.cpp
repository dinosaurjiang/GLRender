//
//  GLBlender.cpp
//  GLRender
//
//  Created by dinosaur on 13-12-9.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#include "GLBlender.h"
#include "GLRenderDef.h"

unsigned int GLBlender::previousSrc = 0;
unsigned int GLBlender::previousDst = 0;


void GLBlender::blend()
{
#if OPEN_GL_CACHE
    // cache function
    if(this->src == GLBlender::previousSrc && this->dst == GLBlender::previousDst)
        return;// because the blend was cached, so we return.
    
    // change blend
    if(src == GL_ONE && dst == GL_ZERO){
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc( src, dst );
    }
    
    //make a flag
    GLBlender::previousSrc = this->src;
    GLBlender::previousDst = this->dst;
#else
    // change blend
    if(src == GL_ONE && dst == GL_ZERO){
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc( src, dst );
    }
#endif
}