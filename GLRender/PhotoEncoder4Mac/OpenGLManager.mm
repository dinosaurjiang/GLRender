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

static OpenGLManager * __manager_instance = nil;

void __my_OpenGLSizeDidChangeCallback(int w, int h)
{
    NSLog(@"view size did changed:%dx%d",w,h);
    GLSupport::openGLViewSizeDidChangeCallback(w, h);
    
    if (__manager_instance)
    {
        [__manager_instance openGLSizeChangeCallbackFunc:w height:h];
    }
#ifdef DEBUG
    else
    {
        NSLog(@"size changed callback:manager is nil");
    }
#endif
}


static void __my_OpenGLDisplayCallback()
{
    if (__manager_instance)
    {
        [__manager_instance openGLDisplayCallbackFunc];
    }
#ifdef DEBUG
    else
    {
        NSLog(@"display callback:manager is nil");
    }
#endif
}

static GLScene * curScene;
static GLSprite * myObject;


@interface OpenGLManager()
-(void)timerCallback:(NSTimer *)timer;
@end


@implementation OpenGLManager

- (void)awakeFromNib
{
    __manager_instance = self;
    
    // then add display call back
    [_openGLView setOpenGLSizeDidChangeCallback:__my_OpenGLSizeDidChangeCallback];
    [_openGLView setOpenGLDisplayCallback:__my_OpenGLDisplayCallback];
    
    if (_refresh_timer == nil)
    {
        _refresh_timer = [NSTimer scheduledTimerWithTimeInterval:FrameInterval
                                                          target:self
                                                        selector:@selector(timerCallback:)
                                                        userInfo:nil
                                                         repeats:YES];
    }
}


-(void)timerCallback:(NSTimer *)timer
{
    [_openGLView redraw];
}


-(void)openGLSizeChangeCallbackFunc:(int)width height:(int)height
{
    _glWidth = width;
    _glHeight = height;
}

-(void)openGLDisplayCallbackFunc
{
    if (curScene == nullptr)
    {
        string tname("THIS IS TEST STRING");
        string tfname("Marker Felt");
        
        myObject = new GLLabel(tname,24.0,tfname,MakeColor4B(255, 0, 0, 255),kGLHTextAlignmentCenter,kGLVTextAlignmentTop,kGLLineBreakModeWordWrap);
//        myObject->setPosition(100, 100);
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


#pragma mark -
#pragma mark opengl view

@implementation OpenGLView

-(void)setOpenGLSizeDidChangeCallback:(OpenGLSizeDidChangeCallback)callback
{
    @synchronized(self)
    {
        _size_change_callback = callback;
    }
}

-(void)setOpenGLDisplayCallback:(OpenGLDisplayCallback)callback
{
    @synchronized(self)
    {
        _display_callback = callback;
    }
}


-(void)awakeFromNib
{
    [[self openGLContext] makeCurrentContext];
    [self  setWantsBestResolutionOpenGLSurface:YES];
    GLSupport::dumpInfo();
    [self viewDidEndLiveResize];
}

-(void)viewDidEndLiveResize
{
    @synchronized(self)
    {
        NSRect backingBounds = [self convertRectToBacking:[self bounds]];
        GLsizei backingPixelWidth  = (GLsizei)(backingBounds.size.width),
        backingPixelHeight = (GLsizei)(backingBounds.size.height);
        
        if(_size_change_callback)
        {
            _size_change_callback(backingPixelWidth, backingPixelHeight);
        }
    }
}

-(void)redraw
{
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    // draw
    if (_display_callback)
    {
        _display_callback();
    }
    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}


@end
