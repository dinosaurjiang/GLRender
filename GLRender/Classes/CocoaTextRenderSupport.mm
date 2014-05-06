//
//  CocoaTextRenderSupport.m
//  GLRender
//
//  Created by dinosaur on 14-5-6.
//  Copyright (c) 2014年 dinosaur. All rights reserved.
//

#import "CocoaTextRenderSupport.h"

#import <Foundation/Foundation.h>
#include "GLRenderDef.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include "vec2.h"
#include "GLSupport.h"
#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import <ImageIO/ImageIO.h>
#import <CoreGraphics/CoreGraphics.h>
#import <AppKit/AppKit.h>


TextureInfo createGLTextureWithString(const char * text,
                                      const char * fontName,
                                      float fontSize,
                                      GLHTextAlignment halign,
                                      GLVTextAlignment valign,
                                      GLLineBreakMode breakMode,
                                      float  boxWidth ,
                                      float boxHeight)
{
    if (text == NULL)
    {
        Exception("empty text", "can not render text with empty string");
    }
    
    //// Abstracted Attributes
    NSString* textContent = [NSString stringWithCString:text
                                               encoding:NSUTF8StringEncoding];
    
    NSFont * textFont = nil;
    
    if (fontName)
    {
        NSString* textFontName = [NSString stringWithCString:fontName
                                                encoding:NSUTF8StringEncoding];
        textFont = [NSFont fontWithName:textFontName size:fontSize];
    }
    
    
    // make sure we have font
    if (textFont == nil)
    {
        [NSFont systemFontOfSize:fontSize];
    }
    
    
    //// Text Drawing
    NSMutableParagraphStyle* textStyle = [[NSMutableParagraphStyle defaultParagraphStyle] mutableCopy];
    [textStyle setAlignment: (NSTextAlignment)halign];
    [textStyle setLineBreakMode:(NSLineBreakMode)breakMode];
    

    NSDictionary* textFontAttributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                        textFont, NSFontAttributeName,
                                        [NSColor whiteColor], NSForegroundColorAttributeName,
                                        textStyle, NSParagraphStyleAttributeName, nil];
    NSSize tsize = {boxWidth, boxHeight};
    if (boxWidth < 1.0 || boxHeight < 1.0)
    {
        // user did not given box to draw.
        // use single line mode
        tsize = [textContent sizeWithAttributes:textFontAttributes];
    }
    
    NSRect textRect = {0,0,tsize.width,tsize.height};

    size_t img_w = tsize.width;
    size_t img_h = tsize.height;
    
    
    size_t width = img_w;
    size_t height = img_h;
    
    if(GLSupport::supportsNPOT() == false)
    {
        width = nextPOTValue(img_w);
        height = nextPOTValue(img_h);
    }
    
    
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    
    //hasPremultipliedAlpha
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    void *imageData = malloc(height * width * 4);
    CGContextRef context = CGBitmapContextCreate( imageData, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
    
    
    
    //#if TARGET_IOS
    CGContextTranslateCTM (context, 0, height);
    CGContextScaleCTM (context, 1.0, -1.0);
    //#endif
    
    // draw string
    CGContextClearRect(context, CGRectMake(0, 0, width, height));
    
    NSGraphicsContext * nsctx = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:NO];
    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:nsctx];
    [textContent drawInRect: textRect withAttributes: textFontAttributes];
    [NSGraphicsContext restoreGraphicsState];

    
    CGContextRelease(context);
    CGColorSpaceRelease( colorSpace );
    
    
    TextureInfo tinfo;
    tinfo.width = (unsigned int)width;
    tinfo.height = (unsigned int)height;
    tinfo._MaxT = (unsigned int)img_w;
    tinfo._MaxS = (unsigned int)img_h;
    
    
    glGenTextures(1, &(tinfo.glName));
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, tinfo.glName );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)width, (int)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    
    free(imageData);
    
    return tinfo;
}

