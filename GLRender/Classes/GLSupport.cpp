//
//  GLSupport.cpp
//  GLRender
//
//  Created by dinosaur on 13-12-9.
//  Copyright (c) 2013 dinosaur. All rights reserved.
//

#include "GLSupport.h"
#include "vec3.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#ifdef __cplusplus
}
#endif


static char * opengl_extensions = NULL;

kmMat4 * GLSupport::projectionMatrix;
kmMat4 * GLSupport::modelViewMatrix;
std::vector<GLSupport::GLMatrixItem *> GLSupport::_matrixStack;
int GLSupport::_GLViewSizeWidth;
int GLSupport::_GLViewSizeHeight;

void GLSupport::openGLViewSize(float * w, float * h)
{
    *w = GLSupport::_GLViewSizeWidth;
    *h = GLSupport::_GLViewSizeHeight;
}

// push a new matrix into stack
void GLSupport::pushMatrix()
{
    GLMatrixItem * mat4 = new GLMatrixItem;
    
    GLSupport::projectionMatrix = &mat4->pMatrix;
    GLSupport::modelViewMatrix = &mat4->mvMatrix;
    
    kmMat4Identity(&mat4->pMatrix);
    kmMat4Identity(&mat4->mvMatrix);
    
    GLSupport::_matrixStack.push_back(mat4);
    
}


// pop matrix
void GLSupport::popMatrix()
{
    
    std::vector<GLMatrixItem *>::size_type size = GLSupport::_matrixStack.size();
    if (size == 1)
    {
        LOG("GLSupport::popMatrix:There's only one matrix on the stack, could not pop out.");
        return;
    }
    
    // remove the last one.
    GLMatrixItem * mat4 = GLSupport::_matrixStack.back();
    delete mat4;
    GLSupport::_matrixStack.pop_back();
    
    // update the current
    mat4 = GLSupport::_matrixStack.back();
    GLSupport::projectionMatrix = &mat4->pMatrix;
    GLSupport::modelViewMatrix = &mat4->mvMatrix;
}


// just 
void GLSupport::resizeMVPMatrixWithSize(float w , float h)
{
    if(GLSupport::_matrixStack.size() == 0)
    {
        // there's no matrix in stack. push a new into it.
        GLSupport::pushMatrix();
    }
    
    // get the current matrix in the stack
    GLMatrixItem * mat4 = GLSupport::_matrixStack.back();
    mat4->sizeWidth = w;
    mat4->sizeHeight = h;
    
    float zeye = h / 1.1566f;
    kmMat4 matrixPerspective, matrixLookup;
    
    // for projection
    kmMat4Identity(GLSupport::projectionMatrix);
    kmMat4PerspectiveProjection( &matrixPerspective, 60.0, w * 1.0 / h, 0.1f, max(zeye*2,1500) );
    kmMat4Multiply(GLSupport::projectionMatrix, GLSupport::projectionMatrix, &matrixPerspective);
    
    
    // for MODELVIEW
    kmMat4Identity(GLSupport::modelViewMatrix);
    kmVec3 eye, center, up;
    
    float eyeZ = h * zeye / h;
    
    kmVec3Fill( &eye, w/2.0, h/2.0, eyeZ );
    kmVec3Fill( &center, w/2.0, h/2.0, 0 );
    kmVec3Fill( &up, 0, 1.0, 0);
    kmMat4LookAt(&matrixLookup, &eye, &center, &up);
    kmMat4Multiply(GLSupport::modelViewMatrix, GLSupport::modelViewMatrix, &matrixLookup);
}

//TODO: does need to mark the w,h ???
void GLSupport::openGLViewSizeDidChangeCallback(float w , float h)
{
    _GLViewSizeWidth = w;
    _GLViewSizeHeight = h;
    GLSupport::resizeMVPMatrixWithSize(w, h);
}


int GLSupport::frameBufferWidth()
{
    return 400;
}

int GLSupport::frameBufferHeight()
{
    return 300;
}

bool GLSupport::supportsNPOT()
{
#ifdef TARGET_IOS
    return true;// ios with GL 2.0 always support NPOT
#else

    
    static int isSupport = -1;
    
    if (isSupport == -1)
    {
        
        if(opengl_extensions == NULL)
        {
            opengl_extensions = (char*) glGetString(GL_EXTENSIONS);
        }
        
        std::string  gl_ext(opengl_extensions);
        std::size_t found = gl_ext.find("GL_ARB_texture_non_power_of_two");

        if(found!=std::string::npos)// is found
        {
            isSupport = 1;
        }
        else
        {
            isSupport = 0;
        }
    }
    return ((isSupport==0)?false:true);
#endif
}


const unsigned char * GLSupport::GL_Version()
{
    return glGetString(GL_VERSION);
}


void GLSupport::GL_Version_Num(int* mainv , int* subv)
{
    int main = 0;
    int sub = 0;
    char * buff[512] = {0};
    const char * version = (const char *)GLSupport::GL_Version();
    sscanf(version, "%d.%d%s",&main,&sub,(char *)buff);
    *mainv = main;
    *subv = sub;
}


void GLSupport::dumpInfo()
{
    // first get info
	GLint			_maxSamplesAllowed = 0;
	GLint			_maxTextureUnits = 0;
	GLint			_maxTextureSize = 0;
#if !TARGET_IOS
    glGetIntegerv(GL_MAX_SAMPLES, &_maxSamplesAllowed);
#endif
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_maxTextureUnits );
    
	printf("GLRender: GL_VENDOR:   %s\n", glGetString(GL_VENDOR) );
	printf("GLRender: GL_RENDERER: %s\n", glGetString ( GL_RENDERER   ) );
	printf("GLRender: GL_VERSION:  %s\n", glGetString ( GL_VERSION    ) );
	printf("GLRender: GL_MAX_TEXTURE_SIZE: %d\n", _maxTextureSize);
	printf("GLRender: GL_MAX_TEXTURE_UNITS: %d\n", _maxTextureUnits);
	printf("GLRender: GL_MAX_SAMPLES: %d\n", _maxSamplesAllowed);
    int m=0,s=0;
    GLSupport::GL_Version_Num(&m, &s);
	printf("GLRender: GL_VERSION_NUMBER: %d.%d\n", m,s);
	printf("GLRender: GL_NPOT_SUPPORT: %s\n", (GLSupport::supportsNPOT()?"YES":"NO"));
}
