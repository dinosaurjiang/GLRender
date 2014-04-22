//
//  MD2Loader.h
//  GLRender
//
//  Created by dinosaur on 13-12-26.
//  Copyright (c) 2013 dinosaur. All rights reserved.
//

#ifndef __GLRender__MD2Loader__
#define __GLRender__MD2Loader__


#include <iostream>
#include "vec2.h"
#include "vec3.h"

//TODO: in the feature...


// comes from http://en.wikipedia.org/wiki/MD2_(file_format)
#ifdef __cplusplus
extern "C" {
#endif
    
    static int MD2_IDENTIFY = 'IDP2';
    
    typedef struct MD2Header
    {
        int ident;
        int version;
        int skinWidth;
        int skinHeight;
        int framesize;
        int num_skins;
        int num_xyz;
        int num_st;
        int num_tris;
        int num_glcmds;
        int num_frames;
        int ofs_skins;
        int ofs_st;
        int ofs_frames;
        int ofs_glcmds;
        int ofs_end;
    }MD2Header;
    
    typedef kmVec3 Vector;
    typedef kmVec2 TexCoord;
    
    
    typedef struct UVIndex
    {
        short U,V;
    }UVIndex;
    
    
    typedef struct Mesh
    {
        unsigned short tMeshIndex[3];
        unsigned short tUVIndex[3];
    }Mesh;
    
    
    typedef struct FramePoint
    {
        unsigned char tVertex[3];
        unsigned char tNormalIndex;
    }FramePoint;
    
    
    typedef struct Frame
    {
        float Scale[3];
        float Translate[3];
        char Name[16];
        FramePoint pFramePoint[1];
    }Frame;
    
    
#ifdef __cplusplus
}
#endif

class MD2Loader {
    
    
public:
    
};

#endif /* defined(__GLRender__MD2Loader__) */
