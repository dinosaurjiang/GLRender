//
//  iOSPvrTextureLoader.h
//  GLRender
//
//  Created by dinosaur on 13-12-3.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef GLRender_iOSPvrTextureLoader_h
#define GLRender_iOSPvrTextureLoader_h

#include "GLRenderDef.h"

// only for ios
#if TARGET_IOS
bool createGLTextureFromPVRFile(const void * data, long length);
#endif

#endif
