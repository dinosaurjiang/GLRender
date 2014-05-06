//
//  GLRenderPlatformHelper.cpp
//  GLRender
//
//  Created by dinosaur on 13-11-29.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#include "GLRenderPlatformHelper.h"
#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import <ImageIO/ImageIO.h>
#import <CoreGraphics/CoreGraphics.h>


#ifdef TARGET_IOS
#include <OpenGLES/ES2/gl.h>
#else
#include <OpenGL/gl.h>
#endif


#include "GLRenderDef.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include "vec2.h"
#include "GLSupport.h"


TextureInfo createGLTextureWithPNGImageDataAPPLE(const void * bytes,long length )
{
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorSystemDefault, (const UInt8 *)bytes, length, kCFAllocatorNull);
    CGImageSourceRef ImgSrc = CGImageSourceCreateWithData(data, NULL);
    CGImageRef image = CGImageSourceCreateImageAtIndex(ImgSrc, 0, NULL);
    
    CFRelease(ImgSrc);
    CFRelease(data);
    
    TextureInfo t = createGLTextureWithCGImage(image);
    CGImageRelease(image);
    
    return t;
}

TextureInfo createGLTextureWithCGImage(void * cgimage)
{
    CGImageRef image = (CGImageRef)cgimage;
    
    
    size_t img_w = CGImageGetWidth(image);
    size_t img_h = CGImageGetHeight(image);
    
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
    
    
    //hasPremultipliedAlpha always yes
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    void *imageData = malloc(height * width * 4);
    CGContextRef context = CGBitmapContextCreate( imageData, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
    
    
    
//#if TARGET_IOS
    CGContextTranslateCTM (context, 0, height);
    CGContextScaleCTM (context, 1.0, -1.0);
//#endif
    
    CGColorSpaceRelease( colorSpace );
    CGContextClearRect(context, CGRectMake(0, 0, width, height));
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
    
    
    CGContextRelease(context);
    
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


TextureInfo createGLTextureWithImageNameAPPLE(std::string & name)
{
    NSString * fileName = [NSString stringWithCString:name.c_str()
                                             encoding:NSUTF8StringEncoding];
    
    NSString *filePath = [[NSBundle mainBundle] pathForResource:fileName
                                                         ofType:nil];
    
    NSData * data = [NSData dataWithContentsOfFile:filePath];
    
    if ([[[fileName pathExtension] lowercaseString] isEqualToString:@"png"])
    {
        return createGLTextureWithPNGImageDataAPPLE([data bytes], [data length]);
    }
    
    if ([[[fileName pathExtension] lowercaseString] isEqualToString:@"pvr"])
    {
        LOG("pvr current not support.");
    }
    return TextureInfo();
}


TextureInfo createGLTextureWithRGBAData(unsigned char * imageData,
                                        long width, long height,
                                        long maxT, long maxS)
{
    TextureInfo tinfo;
    tinfo.width = (unsigned int)width;
    tinfo.height = (unsigned int)height;
    tinfo._MaxT = (unsigned int)maxT;
    tinfo._MaxS = (unsigned int)maxS;
    
    // gen opengl texture with data
    // if is RGBA8888 ||  supportNPOT < is 4 >
    // always RGBA8888 so...
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    
    glGenTextures(1, &(tinfo.glName));
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, tinfo.glName );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)width, (int)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    return tinfo;
}

void * GLReadedRGBADataToCGImage(void * data, int width, int height)
{
    if (data == NULL || width == 0 || height == 0)
    {
        LOG("bad vars to create CGImage");
        return NULL;
    }
    
	int tx = width;
	int ty = height;
	
	int bitsPerComponent			= 8;
    int bitsPerPixel                = 4 * 8;
    int bytesPerPixel               = bitsPerPixel / 8;
	int bytesPerRow					= bytesPerPixel * tx;
	long myDataLength               = bytesPerRow * ty;
    
    
	void *pixels = calloc(myDataLength,1);
    if (pixels == NULL)
    {
        LOG("alloc memory for CGImage failed.");
        return NULL;
    }
    
    
	CGBitmapInfo bitmapInfo	= kCGImageAlphaPremultipliedLast | kCGBitmapByteOrderDefault;
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data, myDataLength, NULL);
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGImageRef iref	= CGImageCreate(tx, ty,
									bitsPerComponent, bitsPerPixel, bytesPerRow,
									colorSpaceRef, bitmapInfo, provider,
									NULL, false,
									kCGRenderingIntentDefault);
	
	CGContextRef context = CGBitmapContextCreate(pixels, tx,
												 ty, CGImageGetBitsPerComponent(iref),
												 CGImageGetBytesPerRow(iref), CGImageGetColorSpace(iref),
												 bitmapInfo);
	
	// vertically flipped
	if( YES ) {
		CGContextTranslateCTM(context, 0.0f, ty);
		CGContextScaleCTM(context, 1.0f, -1.0f);
	}
	CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, tx, ty), iref);
	CGImageRef image = CGBitmapContextCreateImage(context);
	
	CGContextRelease(context);
	CGImageRelease(iref);
	CGColorSpaceRelease(colorSpaceRef);
	CGDataProviderRelease(provider);
	
	free(pixels);
	
	return image;
}


bool saveCGImageToPath(const char * path, void * cgimage)
{
    if (cgimage == NULL || path == NULL)
    {
        return false;
    }
    
	bool success = false;
	
	NSString *fullPath = [NSString stringWithCString:path encoding:NSUTF8StringEncoding];
	
	CGImageRef imageRef =(CGImageRef)cgimage;
    
	if( ! imageRef ) {
		LOG("GLRender: Error: Cannot create CGImage ref from texture");
		return NO;
	}
	
#if TARGET_IOS
	
	UIImage* image	= [[UIImage alloc] initWithCGImage:imageRef scale:2.0 orientation:UIImageOrientationUp];
	NSData *imageData = nil;
    imageData = UIImagePNGRepresentation( image );

	success = [imageData writeToFile:fullPath atomically:YES];
    
	
#elif TARGET_MAC
	
	CFURLRef url = (__bridge CFURLRef)[NSURL fileURLWithPath:fullPath];
	
	CGImageDestinationRef dest;
    
    // default png file
//	if( format == kCCImageFormatPNG )
    dest = 	CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
    
//	else if( format == kCCImageFormatJPEG )
//		dest = 	CGImageDestinationCreateWithURL(url, kUTTypeJPEG, 1, NULL);
//    
//	else
//		NSAssert(NO, @"Unsupported format");
    
	CGImageDestinationAddImage(dest, imageRef, nil);
    
	success = CGImageDestinationFinalize(dest);
    
	CFRelease(dest);
#endif
    
	if( ! success )
		LOG("cocos2d: ERROR: Failed to save file:[%s] to disk",path);
    
	return success;
}


#pragma mark -
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

long loadBundleFileContextWithNameAPPLE(std::string & name, char ** buff)
{
    return loadBundleFileContextWithNameAPPLE(name.c_str(), buff);
}


long loadBundleFileContextWithNameAPPLE(const char * name, char ** buff)
{
    NSString * fileName = [NSString stringWithCString:name
                                             encoding:NSUTF8StringEncoding];
    
    NSString * filePath = [[NSBundle mainBundle] pathForResource:fileName
                                                          ofType:nil];
    
    
    if (filePath == NULL)
    {
        return 0;
    }
    
    long bufsize = 0;
    char *source = NULL;
    FILE *fp = fopen([filePath UTF8String], "r");
    if (fp != NULL) {
        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) {
            /* Get the size of the file. */
            bufsize = ftell(fp);
            if (bufsize == -1) { /* Error */ }
            
            /* Allocate our buffer to that size. */
            source = (char *)malloc(sizeof(char) * (bufsize + 2));
            bzero(source, bufsize+2);
            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }
            
            /* Read the entire file into memory. */
            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if (newLen == 0) {
                fputs("Error reading file", stderr);
            } else {
                
                /*
                 not ++newLen
                 because c was begin with 0.
                 if read 10 char
                 then set buff[10] = '\0'
                 not buff[11]
                 */
                
                source[newLen] = '\0'; /* Just to be safe. */
            }
        }
        fclose(fp);
    }
    
    /* Don't forget to call free() later! */
    //free(source);
    // caller response to release memory
    
    *buff = source;
    
    return bufsize;
}


#import <sys/stat.h>
char *readFile(const char *name, size_t * outLen)
{
	struct stat statbuf;
	FILE *fh;
	char *source;
	
    *outLen = 0;
    
	fh = fopen(name, "r");
	if (fh == 0)
		return 0;
	
	stat(name, &statbuf);
	source = (char *) malloc((size_t)statbuf.st_size + 1);
	fread(source, (size_t)statbuf.st_size, 1, fh);
	source[statbuf.st_size] = '\0';
	fclose(fh);
	
    *outLen = (size_t)statbuf.st_size;
    
	return source;
}


std::string appBundlePath()
{
    NSString * bstr = [[NSBundle mainBundle] bundlePath];
    return std::string([bstr UTF8String]);
}

std::string appResBundlePath()
{
    NSString * bstr = [[NSBundle mainBundle] resourcePath];
    return std::string([bstr UTF8String]);
}

float scaleFactor()
{
    static float __scale_factor = -1.0f;
    if (__scale_factor < 0)
    {
#if TARGET_IOS
        __scale_factor = [UIScreen scale];
#elif TARGET_MAC
        __scale_factor = [[NSScreen mainScreen] backingScaleFactor];
#else
        __scale_factor = 1.0f;
#endif
    }
    return __scale_factor;
}
