//
//  OpenGLManager.h
//  GLRender
//
//  Created by dinosaur on 13-12-30.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

#import <Foundation/Foundation.h>


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
@class OpenGLView;
@protocol OpenGLManagerProtocal <NSObject>

@required
-(void) openGLViewDidReady:(OpenGLView *)glview;
-(void)openGLSizeChangeCallbackFunc:(CGFloat)width height:(CGFloat)height;
-(void)openGLDisplayCallbackFunc;

@end


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

@class OpenGLView;
@interface OpenGLManager : NSObject <OpenGLManagerProtocal>
{
    NSTimer * _refresh_timer;
    CGFloat _glWidth,_glHeight;
}

@property (nonatomic, assign) IBOutlet OpenGLView *  openGLView;

@end

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

@interface OpenGLView : NSOpenGLView

@property (assign) id<OpenGLManagerProtocal> manager;

-(Class)managerClass;
-(void)redraw;

@end

