//
//  GLTexture.h
//  GLRender
//
//  Created by dinosaur on 13-11-28.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#ifndef __GLRender__GLTexture__
#define __GLRender__GLTexture__

#include <iostream>
#include "vec4.h"
#include "vec2.h"
#include <map>
#include "GLRenderPlatformHelper.h"
#include "GLBase.h"

using namespace std;


/*
 this is texture obj for gl
 */

class GLTexture;
class GLTextureName : public GLBase
{
public:
    
    DECLARE_CLASS(GLTextureName)
    
    unsigned int height()
    {
        return _height;
    };
    
    unsigned int width()
    {
        return _width;
    };
    
    unsigned int textureName()
    {
        return _textureName;
    };
    
    void retain()
    {
        _refCounts++;
    }
    
    void release()
    {
        _refCounts--;
    }
    
    bool operator==(GLTextureName * tex)
    {
        return (_textureName == tex->_textureName);
    }
    
private:
    
    ~GLTextureName();
    GLTextureName( TextureInfo & info)
    {
        _width = info.width;
        _height = info.height;
        _textureName = info.glName;
        
        _maxT = info._MaxT;
        _maxS = info._MaxS;
        
        isPowerOfTwo = ((_width == _maxT) && (_height == _maxS));
    };

    unsigned int refCount()
    {
        return _refCounts;
    }
    
    unsigned int _refCounts=1;
    unsigned int _width = 0;
    unsigned int _height = 0;
    unsigned int _textureName = 0;
    
    unsigned int _maxT;
    unsigned int _maxS;
    
    bool isPowerOfTwo;
    
    friend class GLTexture;
};


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

class GLTexture : public GLBase
{
    
public:
    
    DECLARE_CLASS(GLTexture)
    
    static GLTexture * createTextureWithImageName(const char *  c_name);
    static GLTexture * createTextureWithImageName(string & name);
    static GLTexture * createTextureWithPNGDataAndName(const void * bytes,
                                                       unsigned long length,
                                                       string & name);

    GLTexture(TextureInfo info);
    ~GLTexture();
    
    // those texture was create out-off texture manager.
    // suggest to cache it at manager
    // if not , you need to delete it by your self.
    GLTexture * subTextureWithFrame(kmVec4 frame);
    GLTexture * copy();
    
    void bind();
    void unbind();
    
    //  这里width 和height  是图片的真正的size
    int texWidth()
    {
        return _gltexture->width();
    }
    
    int texHeight()
    {
        return _gltexture->height();
    }
    
    kmVec4 frame()
    {
        return _frame;
    }
    
    
    // 这里是整个纹理的大小，
    // 因为要兼容2的n次方(电脑的显卡决定).
    // 所以纹理实际的大小可能比图片的大小大。
    
    // width
    int maxT()
    {
        return _gltexture->_maxT;
    }
    // height
    int maxS()
    {
        return _gltexture->_maxS;
    }
    
    unsigned int textureGLName()
    {
        if(_gltexture)
            return _gltexture->textureName();
        return 0;
    }
    
    GLTexture * retain();
    
    void setAliasTexParameters();
    void setAntiAliasTexParameters();
    
protected:
    GLTextureName*      _gltexture = nullptr;
    kmVec4              _frame;
    bool                _hasMipmaps = false;
    
    GLTexture(GLTextureName * info);
    
    // for caching.
    static unsigned int previousTexture;
};


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// texture manager.
// should always load texture from here.
// if have none-managed texture, pls cache it here, or delete by youself

class GLTextureManager : public GLBase
{
public:
    
    DECLARE_CLASS(GLTextureManager)
    
    bool cacheTexture(GLTexture * texture,const char * cname);
    bool cacheTexture(GLTexture * texture,string & name);
    
    bool removeCacheTexture(GLTexture * texture);
    bool removeCacheTexture(const char * cname);
    bool removeCacheTexture(string & name);
    
    void clearAllCache();
    
    GLTexture * getTextureWithName(string & name);
    
    static GLTextureManager * manager()
    {
        static GLTextureManager _manager_instance;
        return &_manager_instance;
    }
    
private:
    map<string, GLTexture *> _textureList;
};


#endif /* defined(__GLRender__GLTexture__) */
