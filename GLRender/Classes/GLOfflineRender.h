//
//  GLOfflineRender.h
//  GLRender
//
//  Created by dinosaur on 13-12-19.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

#ifndef __GLRender__GLOfflineRender__
#define __GLRender__GLOfflineRender__

#include "GLBase.h"

#ifdef TARGET_IOS
#include <OpenGLES/ES2/gl.h>
#else
#include <OpenGL/gl.h>
#endif


/*
 OpenGL 离线渲染的类。
 
 所有的 object 可以在这个类里面渲染。
 这样做的好处是可以截图。等等。。。
 
 all the render call must 
 inside at:
 
 beginRender() 
 ...
 your gl draw commands.
 ...
 endRender()
 
 
 object was render to pixels format:RGBA.
 
 */

class GLTexture;
class GLOfflineRender : public GLBase
{
public:
    
    DECLARE_CLASS(GLOfflineRender)
    
    ~GLOfflineRender();
    GLOfflineRender(unsigned int width, unsigned int height);
    
    
    void begin();
    void beginAndClean(float r, float g, float b);
    void end();
    
    // caller response to free data.
    void * renderRGBAResult(long * );
    // caller response to delete the texture.
    GLTexture * renderToTexture();
    
    
    // this just keep canvas texture
    // caller response to delete it
    GLTexture * keepTexture();
    
private:
    
    
    void * renderResult(long * length, GLenum target_pix_type );
    
    unsigned int        _width;
    unsigned int        _height;
    
    GLTexture           *_canvasTexture;
	GLuint				_FBO;
	GLint				_oldFBO;
	GLuint				_depthRenderBufffer;
};


#endif /* defined(__GLRender__GLOfflineRender__) */
