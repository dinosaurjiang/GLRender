//
//  CocoaTextRenderSupport.h
//  GLRender
//
//  Created by dinosaur on 14-5-6.
//  Copyright (c) 2014年 dinosaur. All rights reserved.
//

#ifndef __GLRender__CocoaTextRenderSupport__
#define __GLRender__CocoaTextRenderSupport__

#include <iostream>
#include "Utilities.h"

// texture color was white
TextureInfo createGLTextureWithString(const char * text,
                                      const char * fontName,
                                      float fontSize,
                                      GLHTextAlignment halign,
                                      GLVTextAlignment valign,
                                      GLLineBreakMode breakMode,
                                      float  boxWidth ,
                                      float boxHeight);

#endif
