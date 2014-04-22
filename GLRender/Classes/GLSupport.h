//
//  GLSupport.h
//  GLRender
//
//  Created by dinosaur on 13-12-9.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef __GLRender__GLSupport__
#define __GLRender__GLSupport__

#include <iostream>
#include "mat4.h"
#include "vector"

class GLSupport
{
    
public:
    static kmMat4 * projectionMatrix;
    static kmMat4 * modelViewMatrix;
    
    static void pushMatrix();
    static void popMatrix();
    
    static int frameBufferWidth();
    static int frameBufferHeight();
    
    static void openGLViewSizeDidChangeCallback(float w , float h);
    static void resizeMVPMatrixWithSize(float w , float h);
    static void openGLViewSize(float * w, float * h);
    
    static bool supportsNPOT();
    static const unsigned char * GL_Version();
    static void GL_Version_Num(int* mainv , int* subv);
    
    static void dumpInfo();
    
private:
    
    class GLMatrixItem
    {
    public:
        ~GLMatrixItem()=default;
        GLMatrixItem()=default;
        kmMat4 pMatrix;
        kmMat4 mvMatrix;
        float sizeWidth;
        float sizeHeight;
    };
    
    static std::vector<GLMatrixItem *> _matrixStack;
    
    static int _GLViewSizeWidth;
    static int _GLViewSizeHeight;
    
    GLSupport() = default;
    ~GLSupport() = default;
};


#endif /* defined(__GLRender__GLSupport__) */
