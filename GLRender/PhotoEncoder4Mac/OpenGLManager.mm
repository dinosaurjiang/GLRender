//
//  OpenGLManager.m
//  GLRender
//
//  Created by dinosaur on 13-12-30.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

/*
 {
 GLOfflineRender * render = new GLOfflineRender(w,h);
 render->begin();
 curScene->draw();
 render->end();
 
 GLTexture * tmp = render->renderToTexture();
 delete render;
 
 curScene->removeAllChildren(true);
 
 myObject = new GLSprite(0,0,300,200);
 GLTextureManager::manager()->cacheTexture(tmp, "render");
 myObject->setTexture(tmp);
 curScene->addChild(myObject);
 curScene->setColor(MakeColor4B(0, (unsigned char)(255 * randomFloat()) , 0, 255));
 
 GLActionRotateZBy * rotateZ = new GLActionRotateZBy(myObject,100,3,360);
 rotateZ->actionTimingFunction = kTransiltionTimingFunctionEaseInOut;
 GLObjectActionManager::manager()->addAction(rotateZ);
 GLObjectActionManager::manager()->setActionDoneCallBack(finished_call_back);
 }
 */

#import "OpenGLManager.h"
#import <OpenGL/OpenGL.h>

#include "GLRenderPlatformHelper.h"
#include "GLProgram.h"
#include "GLObject.h"
#include "GLTexture.h"
#include "GLRenderDef.h"
#include "mat4.h"
#include "Utilities.h"
#include "GLSupport.h"
#include "GLObjectAction.h"
#include "GLParticleSystems.h"
#include "GLLabel.h"
#include "GLOfflineRender.h"
#include "GLScene.h"

#define FrameRate 60.0
#define FrameInterval (1.0/FrameRate)


static GLScene * curScene;
static GLLabel * myObject;


@interface OpenGLManager()
-(void)timerCallback:(NSTimer *)timer;
@end


@implementation OpenGLManager


-(void)timerCallback:(NSTimer *)timer
{
    [_openGLView redraw];
}

-(void) openGLViewDidReady:(OpenGLView *)glview
{
    self.openGLView = glview;
    if (_refresh_timer == nil)
    {
        _refresh_timer = [NSTimer scheduledTimerWithTimeInterval:FrameInterval
                                                          target:self
                                                        selector:@selector(timerCallback:)
                                                        userInfo:nil
                                                         repeats:YES];
    }
}

-(void)openGLSizeChangeCallbackFunc:(CGFloat)width height:(CGFloat)height
{
    _glWidth = width;
    _glHeight = height;
    GLSupport::openGLViewSizeDidChangeCallback(width, height);
}

-(void)openGLDisplayCallbackFunc
{
    if (curScene == nullptr)
    {
        string tname("THIS IS TEST STRING");
        string tfname("Marker Felt");
        
        myObject = new GLLabel();
        myObject->init(tname,tfname,24.0,MakeColor4B(255, 0, 0, 255),kGLHTextAlignmentCenter,kGLVTextAlignmentTop,kGLLineBreakModeWordWrap);
        myObject->setPosition(100, 100);
        myObject->setZDepth(2);
        
        curScene = new GLScene();
        curScene->addChild(myObject);
    }
    
    // 清除屏幕
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0, 0, _glWidth, _glHeight);
    
    GLObjectActionManager::manager()->stepIn(FrameInterval);// xx sec
    curScene->update(FrameInterval);
    
    curScene->draw();
    
    glFlush();
}

@end


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#pragma mark -
#pragma mark opengl view

static OpenGLManager * _manager_instance_ = nil;


@implementation OpenGLView

-(Class)managerClass
{
    return [OpenGLManager class];
}

-(void)awakeFromNib
{
    if (_manager_instance_ == nil)
    {
        _manager_instance_ = [[[self managerClass] alloc] init];
        self.manager = _manager_instance_;
    }
    
    [[self openGLContext] makeCurrentContext];
    [self  setWantsBestResolutionOpenGLSurface:YES];
    GLSupport::dumpInfo();
    [self viewDidEndLiveResize];
    [self.manager openGLViewDidReady:self];
}

-(void)viewDidEndLiveResize
{
    @synchronized(self)
    {
        NSRect backingBounds = [self convertRectToBacking:[self bounds]];
        GLsizei backingPixelWidth  = (GLsizei)(backingBounds.size.width),
        backingPixelHeight = (GLsizei)(backingBounds.size.height);
        
        [self.manager openGLSizeChangeCallbackFunc:backingPixelWidth
                                            height:backingPixelHeight];
    }
}

-(void)redraw
{
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    // draw
    [_manager openGLDisplayCallbackFunc];
    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}


@end
