//
//  GLRenderDef.h
//  GLRender
//
//  Created by dinosaur on 13-11-29.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef GLRender_GLRenderDef_h
#define GLRender_GLRenderDef_h


#if DEBUG
    #define LOG(format , ...) printf(format "\n" , ## __VA_ARGS__)
#else
    #define LOG(format, ...) do{}while(0)
#endif


#define OPEN_GL_CACHE 1

#ifdef _WIN64

    #define TARGET_WINDOWS 1
    #define TARGET_WINDOWS_64 1

#elif _WIN32

    #define TARGET_WINDOWS 1
    #define TARGET_WINDOWS_32 1

#elif __APPLE__

#include "TargetConditionals.h"

    #if TARGET_IPHONE_SIMULATOR

        #define TARGET_IOS 1
        #define TARGET_IOS_SIMULATOR 1

    #elif TARGET_OS_IPHONE

        #define TARGET_IOS 1
        #define TARGET_IOS_DEVICE 1

    #elif TARGET_OS_MAC

        #define TARGET_MAC 1

    #else

        // UNKNOW

    #endif

#elif __linux
    #define TARGET_LINUX
#elif __unix
    #define TARGET_UNIX
#elif __posix
    #define TARGET_POSIX
#endif




#ifdef TARGET_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif TARGET_MAC
#include <OpenGL/gl.h>
#endif


#if TARGET_IOS

    #define	glClearDepth				glClearDepthf
    #define glDeleteVertexArrays		glDeleteVertexArraysOES
    #define glGenVertexArrays			glGenVertexArraysOES
    #define glBindVertexArray			glBindVertexArrayOES
    #define glMapBuffer					glMapBufferOES
    #define glUnmapBuffer				glUnmapBufferOES

    #define GL_DEPTH24_STENCIL8			GL_DEPTH24_STENCIL8_OES
    #define GL_WRITE_ONLY				GL_WRITE_ONLY_OES

#elif TARGET_MAC

    #if 1 // for 2.0+
        #define glDeleteVertexArrays		glDeleteVertexArraysAPPLE
        #define glGenVertexArrays			glGenVertexArraysAPPLE
        #define glBindVertexArray			glBindVertexArrayAPPLE
    #else // OpenGL 3.2 Core Profile
        #define glDeleteVertexArrays		glDeleteVertexArrays
        #define glGenVertexArrays			glGenVertexArrays
        #define glBindVertexArray			glBindVertexArray
    #endif

#endif




#define PI       3.14159265359f
#define PI_HALF  1.57079632679f
#define TWO_PI   6.28318530718f


#define ProjectBuildTime [[[[NSString stringWithFormat:@"%s", __DATE__] stringByReplacingOccurrencesOfString:@"  " withString:@"-"] \
stringByReplacingOccurrencesOfString:@" " withString:@"-"] stringByAppendingFormat:@" %s", __TIME__]




#ifndef Exception
#define Exception(kind, args... ) \
do { printf("%s[line:%d],func:%s\n\t",__FILE__,__LINE__,__FUNCTION__);MyException(kind, args); }while(0)
#endif


#define \
GLAssert(EXP ,reasen ... ) \
if(!(EXP)){ printf("%s[line:%d],func:%s\n\t",__FILE__,__LINE__,__FUNCTION__);MyException("ASSERT FAILED:", reasen); }


#define CHECK_GL_ERROR() ({ GLenum __error = glGetError(); if(__error) printf("OpenGL error 0x%04X in %s %d\n", __error, __FUNCTION__, __LINE__); })



#if !defined(MIN)
#define MIN(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#endif

#if !defined(MAX)
#define MAX(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })
#endif

#if !defined(ABS)
#define ABS(A)	({ __typeof__(A) __a = (A); __a < 0 ? -__a : __a; })
#endif


#if !defined(MIN)
#define MIN(A,B)	((A) < (B) ? (A) : (B))
#endif

#if !defined(MAX)
#define MAX(A,B)	((A) > (B) ? (A) : (B))
#endif


#define VALUE_SWAP( x, y ) ({ __typeof__(x) temp  = (x);x = y; y = temp;})




#endif
