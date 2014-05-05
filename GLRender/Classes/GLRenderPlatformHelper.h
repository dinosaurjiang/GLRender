//
//  GLRenderPlatformHelper.h
//  GLRender
//
//  Created by dinosaur on 13-11-29.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef __GLRender__GLRenderPlatformHelper__
#define __GLRender__GLRenderPlatformHelper__

#include <iostream>
#include "Utilities.h"

// here is only for apple
TextureInfo createGLTextureWithPNGImageDataAPPLE(const void * bytes,long length );
TextureInfo createGLTextureWithCGImage(void * cgimage);
TextureInfo createGLTextureWithImageNameAPPLE(std::string & name);
TextureInfo createGLTextureWithRGBAData(unsigned char * data, long width, long height, long maxT, long maxS);


// texture color was white
TextureInfo createGLTextureWithString(std::string & text,
                                      float fontSize,
                                      std::string & fontName,
                                      GLHTextAlignment halign,
                                      GLVTextAlignment valign,
                                      GLLineBreakMode breakMode,
                                      float  boxWidth ,
                                      float boxHeight);


// caller response to release the CGImage
void * GLReadedRGBADataToCGImage(void * data, int width, int height);
bool saveCGImageToPath(const char * path, void * cgimage);


long loadBundleFileContextWithNameAPPLE(const char * name, char ** buff);
long loadBundleFileContextWithNameAPPLE(std::string & name, char ** buff);
char *readFile(const char *name, size_t * outLen);


float scaleFactor();


std::string appBundlePath();
std::string appResBundlePath();

#endif /* defined(__GLRender__GLRenderPlatformHelper__) */
