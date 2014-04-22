//
//  OpenGLManager.h
//  GLRender
//
//  Created by dinosaur on 13-12-30.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

#import <Foundation/Foundation.h>

@class OpenGLView;
@interface OpenGLManager : NSObject
{
    NSTimer * _refresh_timer;
    int _glWidth,_glHeight;
}

@property (nonatomic, assign) IBOutlet OpenGLView *  openGLView;

-(void)openGLSizeChangeCallbackFunc:(int)width height:(int)height;
-(void)openGLDisplayCallbackFunc;

@end




typedef void (*OpenGLSizeDidChangeCallback)(int w, int h);
typedef void (*OpenGLDisplayCallback)();


@interface OpenGLView : NSOpenGLView
{
    OpenGLDisplayCallback _display_callback;
    OpenGLSizeDidChangeCallback _size_change_callback;
}

-(void)setOpenGLDisplayCallback:(OpenGLDisplayCallback)callback;
-(void)setOpenGLSizeDidChangeCallback:(OpenGLSizeDidChangeCallback)callback;

-(void)redraw;
@end

