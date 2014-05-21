//
//  GLTexture.cpp
//  GLRender
//
//  Created by dinosaur on 13-11-28.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#include "GLTexture.h"
#include "Utilities.h"
#include "GLRenderPlatformHelper.h"
#include "GLRenderDef.h"

unsigned int GLTexture::previousTexture = 0;

GLTextureName::~GLTextureName()
{
    LOG("delete gl_texture_name:%d", _textureName );
    glDeleteTextures(1, &_textureName);
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


GLTexture * GLTexture::retain()
{
    _gltexture->retain();
    return this;
}


GLTexture::~GLTexture()
{
    if(_gltexture)
        _gltexture->release();
    
    LOG( "delete texture" );
    
    // none to use it. delete it.
    if(_gltexture->refCount() == 1)
    {
        LOG( "texture name ref == 1 --> [DELETE]" );
        delete _gltexture;
    }
}

GLTexture::GLTexture(GLTextureName * name)
{
    this->_gltexture = name;
    kmVec4Fill(&(this->_frame),0, 0, name->width(), name->height());
    name->retain();
}

GLTexture::GLTexture(TextureInfo info)
{
    if(info.width == 0 || info.height == 0 || info.glName == 0)
    {
        Exception("Invalied texture info", "the texture info is empty");
    }
    
    GLTextureName * name = new GLTextureName(info);
    this->_gltexture = name;
    kmVec4Fill(&(this->_frame),0, 0, name->width(), name->height());
    name->retain();
}

GLTexture * GLTexture::subTextureWithFrame(kmVec4 frame)
{
    GLTexture * newObj = new GLTexture(_gltexture);
    if(!newObj) return nullptr;
    newObj->_frame = frame;
    return newObj;
}

GLTexture * GLTexture::copy()
{
    GLTexture * newObj = new GLTexture(_gltexture);
    if(!newObj) return nullptr;
    newObj->_frame = this->frame();
    return newObj;
}

void GLTexture::bind()
{
#if OPEN_GL_CACHE
    // is bind
    if(GLTexture::previousTexture == _gltexture->_textureName)
        return;// last time bind, so , just return
    
    // now, bind
    glBindTexture(GL_TEXTURE_2D, _gltexture->_textureName);
    
    // make a flag
    GLTexture::previousTexture = _gltexture->_textureName;
#else
    glBindTexture(GL_TEXTURE_2D, _gltexture->_textureName);
#endif
}

void GLTexture::unbind()
{
    GLTexture::previousTexture = 0;
    glBindTexture(GL_TEXTURE_2D, 0);
}


void GLTexture::setAliasTexParameters()
{
    this->bind();
	
	if( ! _hasMipmaps )
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    else
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}

void GLTexture::setAntiAliasTexParameters()
{
    this->bind();
	if( ! _hasMipmaps )
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    else
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


GLTexture * GLTexture::createTextureWithImageName(const char *  c_name)
{
    string name(c_name);
    return GLTexture::createTextureWithImageName(name);
}
GLTexture * GLTexture::createTextureWithImageName(string & name)
{
    TextureInfo info =  createGLTextureWithImageNameAPPLE(name);
    GLTexture * texture = new GLTexture(info);
    if (!texture)
    {
        return nullptr;
    }
    return texture;
}

GLTexture * GLTexture::createTextureWithPNGDataAndName(const void * bytes,
                                                       unsigned long length,
                                                       string & name)
{
    TextureInfo info =  createGLTextureWithPNGImageDataAPPLE(bytes,length);
    GLTexture * texture = new GLTexture(info);
    if (!texture)
    {
        return nullptr;
    }
    return texture;
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



GLTexture * GLTextureManager::getTextureWithName(string & name)
{
    map<string, GLTexture *>::iterator it = _textureList.find(name);
    if(it == _textureList.end()) return nullptr;
    return it->second;
}

bool GLTextureManager::cacheTexture(GLTexture * texture,const char * cname)
{
    string cppName(cname);
    return this->cacheTexture(texture, cppName);
}

bool GLTextureManager::cacheTexture(GLTexture * texture,string & name)
{
    GLAssert(texture!=nullptr, "Can not cache NULL texture");
    
    
    if (texture)
    {
        /////
        ///  if have a texture store as name, what to do ????
        GLTexture * temp = this->getTextureWithName(name);
        
        // if already cache by name
        // and texure is not cached one
        // remove the previous
        if(temp)
        {
            if ( temp != texture)
            {
                // delete old one
                LOG("found previous texture cache as name<%s>, here we delete previous one",name.c_str());
                delete temp;
            }
            else
            {
                // the texture was already cached. do nothing.
                return true;
            }
        }
        _textureList[name] = texture;
        return true;
    }
    return false;
}

bool GLTextureManager::removeCacheTexture(GLTexture * texture)
{
    if(texture == nullptr) return false;
    
    map<string, GLTexture *>::iterator map_it = _textureList.begin();
    while (map_it != _textureList.end())
    {
        if (texture == map_it->second)
        {
            GLTexture* tmp = map_it->second;
            _textureList.erase(map_it);
            delete tmp;
            return true;
        }
        map_it++;
    }
    
    LOG("Can not find texture< %p >,so we delete texture directly.",texture);
    delete texture;
    return true;
}


bool GLTextureManager::removeCacheTexture(const char * cname)
{
    if(cname == nullptr) return false;
    string cppName(cname);
    return this->removeCacheTexture(cppName);
}


bool GLTextureManager::removeCacheTexture(string & name)
{
    GLTexture * tmp = this->getTextureWithName(name);
    if (tmp)
    {
        delete tmp;
        _textureList[name] = nullptr;
        return true;
    }
    LOG( "Can not find texture with name:%s" , name.c_str() );
    return false;
}


void GLTextureManager::clearAllCache()
{
    map<string, GLTexture *>::iterator map_it = _textureList.begin();
    while (map_it != _textureList.end())
    {
        delete map_it->second;
        map_it++;
    }
    
    _textureList.clear();
}
