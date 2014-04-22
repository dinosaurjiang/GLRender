//
//  GLOfflineRender.cpp
//  GLRender
//
//  Created by dinosaur on 13-12-19.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

#include "GLOfflineRender.h"
#include "GLTexture.h"
#include "GLSupport.h"
#include "Utilities.h"

/* for Mac OS Documents
 *https://developer.apple.com/library/mac/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_offscreen/opengl_offscreen.html#//apple_ref/doc/uid/TP40001987-CH403-SW2
 */

GLOfflineRender::~GLOfflineRender()
{
    LOG("GLOfflineRender deleted, will release its texture also.");
    // when delete, also delete the texture.
    // if you want to keep texture.
    // keep call keepTexture() function to get a copy texture.
    GLTextureManager::deleteTexture(_canvasTexture);
	glDeleteFramebuffers(1, &_FBO);
	if (_depthRenderBufffer)
		glDeleteRenderbuffers(1, &_depthRenderBufffer);
}

GLOfflineRender::GLOfflineRender(unsigned int width, unsigned int height)
{
    _width = width;
    _height = height;
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFBO);
    
    unsigned long powW;
    unsigned long powH;

    if (GLSupport::supportsNPOT())
    {
        powW = _width;
        powH = _height;
    } else {
        powW = nextPOTValue(_width);
        powH = nextPOTValue(_height);
    }
    
    
    void *data = malloc((int)(powW * powH * 4));
    memset(data, 0, (int)(powW * powH * 4));
  
    TextureInfo  texInfo = createGLTextureWithRGBAData((unsigned char*)data,
                                                       _width, _height,
                                                       powW, powH);
    _canvasTexture = new GLTexture(texInfo);
    free(data);
    
    
    
    GLint oldRBO;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRBO);
    
    
    // generate FBO
    glGenFramebuffers(1, &_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GLTextureManager::textureName(_canvasTexture),0);
    
    // default did not use depth buff
    GLuint depthStencilFormat = 0;
    if (depthStencilFormat != 0)
    {
        //create and attach depth buffer
        glGenRenderbuffers(1, &_depthRenderBufffer);
        glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBufffer);
        glRenderbufferStorage(GL_RENDERBUFFER, depthStencilFormat, (GLsizei)powW, (GLsizei)powH);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBufffer);
        
        
        // if depth format is the one with stencil part,
        // bind same render buffer as stencil attachment
#if TARGET_MAC
        if (depthStencilFormat == GL_DEPTH24_STENCIL8)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBufffer);
#endif
    }
    
    
    // check if it worked (probably worth doing :) )
    GLAssert( glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Could not attach texture to framebuffer");
    
    //setAliasTexParameters
    _canvasTexture->setAliasTexParameters();
    
    glBindRenderbuffer(GL_RENDERBUFFER, oldRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _oldFBO);
}

void GLOfflineRender::begin()
{
    GLOfflineRender::beginAndClean(0, 0, 0);
}

void GLOfflineRender::beginAndClean(float r, float g, float b)
{
    
    GLSupport::pushMatrix();
    GLSupport::resizeMVPMatrixWithSize(_width, _height);
    
    glClearColor(r, g, b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT );
    
    glViewport(0, 0, _canvasTexture->texWidth(), _canvasTexture->texHeight());
    
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
}


void GLOfflineRender::end()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _oldFBO);
    
    float vw,vh;
    GLSupport::popMatrix();
    GLSupport::openGLViewSize(&vw, &vh);
    GLSupport::openGLViewSizeDidChangeCallback(vw, vh);
}

GLTexture * GLOfflineRender::renderToTexture()
{
	int tx = _width;
	int ty = _height;
    long dataLen = 0;
    
    GLTexture * ret = nullptr;
    if (GLSupport::supportsNPOT() || (tx == nextPOTValue(tx) && ty == nextPOTValue(ty)))
    {
        void * rgbaData = this->renderResult(&dataLen, GL_RGBA);
        TextureInfo  texInfo = createGLTextureWithRGBAData((unsigned char*)rgbaData,
                                                           tx, ty,
                                                           tx, ty);
        ret = new GLTexture(texInfo);
        free(rgbaData);
    }
    else
    {
        Exception("Need implements", "");
    }
    return ret;
}

void * GLOfflineRender::renderRGBAResult( long * dataLen)
{
    return this->renderResult(dataLen, GL_RGBA);
}

void * GLOfflineRender::renderResult(long * length, GLenum target_pix_type )
{
	int tx = _width;
	int ty = _height;
	
    int bitsPerPixel                = 4 * 8;
    int bytesPerPixel               = bitsPerPixel / 8;
	int bytesPerRow					= bytesPerPixel * tx;
	long myDataLength               = bytesPerRow * ty;
	
	void *buffer = calloc(myDataLength,1);
    
    if (!buffer)// alloc memory error.
    {
        *length = 0;
        return NULL;
    }
    
    this->begin();
	glReadPixels(0,0,tx,ty,target_pix_type,GL_UNSIGNED_BYTE, buffer);
    this->end();
    
    *length = myDataLength;
    
    return buffer;
}

GLTexture * GLOfflineRender::keepTexture()
{
    if(_canvasTexture)
        return _canvasTexture->copy();
    return nullptr;
}
