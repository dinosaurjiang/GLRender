//
//  Utilities.h
//  GLRender
//
//  Created by dinosaur on 13-12-3.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef GLRender_Utilities_h
#define GLRender_Utilities_h

#include <iostream>
#include "GLRenderDef.h"

struct TextureInfo
{
    unsigned int width;
    unsigned int height;
    unsigned int _MaxT;// w
    unsigned int _MaxS;//h
    unsigned int glName;
};

typedef enum
{
	kGLHTextAlignmentLeft,
	kGLHTextAlignmentRight,
	kGLHTextAlignmentCenter,
} GLHTextAlignment;


typedef enum
{
    kGLVTextAlignmentTop,
    kGLVTextAlignmentCenter,
    kGLVTextAlignmentBottom,
} GLVTextAlignment;


typedef enum {
	kGLLineBreakModeWordWrap,
	kGLLineBreakModeCharacterWrap,
	kGLLineBreakModeClip,
	kGLLineBreakModeHeadTruncation,
	kGLLineBreakModeTailTruncation,
	kGLLineBreakModeMiddleTruncation
} GLLineBreakMode;


unsigned long nextPOTValue(unsigned long x);


/** @def CCRANDOM_0_1
 returns a random float between 0 and 1
 #define CCRANDOM_0_1() ((random() / (float)0x7fffffff ))
 */
// [0-1]
static inline float randomFloat()
{
    return (float) arc4random() / UINT_MAX;
}

// [-1 - 1]
static inline float randomFloat_Minus1_1()
{
    return ((random() / (float)0x3fffffff )-1.0f);
}

static inline double cppString2Float(std::string & str)
{
    double temp = ::atof(str.c_str());
    return temp;
}

std::string string_format(const char *fmt, ...);

bool hasPrefix(std::string & str, const std::string & prefix);

bool hasSuffix(std::string & str, const std::string & suffix);

std::string lastPathComponent(std::string & path);


class RenderException
{
    std::string ename;
    std::string reason;
    
public:
    
    RenderException(std::string & n,std::string & r)
    {
        ename = n;
        reason = r;
    }
    
    void print()
    {
        std::cout <<"GLException:" << ename << ",reason:" << reason << std::endl;
    }
    
};

void MyException(const char * kind,
                 const char *fmt, ...);



static inline float radius_to_angle(float radius)
{
    return (radius * 57.29577951f);//57.29577951f -> // PI * 180
}

static inline float angle_to_radius(float angle)
{
    return (angle * 0.01745329252f);//0.01745329252f -> PI / 180
}


static inline float clampf(float value, float min_inclusive, float max_inclusive)
{
	if (min_inclusive > max_inclusive) {
		VALUE_SWAP(min_inclusive,max_inclusive);
	}
	return value < min_inclusive ? min_inclusive : value < max_inclusive? value : max_inclusive;
}


static inline long long pointer_2_integer(void * param)
{
    long long t = (intptr_t)param;
    return t;
}

static inline void * integer_2_pointer(long long t)
{
    void * tp = (void *)t;
    return tp;
}




#ifndef TARGET_IOS
int load_png_texture(const char* filename,
                     int* width,
                     int* height,
                     float* tex_x,
                     float* tex_y,
                     unsigned int *tex);
#endif

#endif
