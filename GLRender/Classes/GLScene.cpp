//
//  GLScene.cpp
//  GLRender
//
//  Created by dinosaur on 13-12-24.
//  Copyright (c) 2013 dinosaur. All rights reserved.
//

#include "GLScene.h"

#include "GLSupport.h"
#include "GLTexture.h"
#include "GLProgram.h"
#include "GLBlender.h"
#include "GLOfflineRender.h"
#include "GLRenderDef.h"
#include "Utilities.h"
#include "GLRenderPlatformHelper.h"
#include <stdarg.h>

// lua writer is kidding me.
// he did not consider include for c++.
#ifdef __cplusplus
extern "C" {
#endif
    
#include "lua.h"
#include "lualib.h"
#include "luaconf.h"
#include "lauxlib.h"
    
#ifdef __cplusplus
}
#endif


        
using namespace std;

GLScene::Data::~Data()
{
    if(_mem) delete [] _mem;
    if(_dataName) delete _dataName;
};

GLScene::Data::Data(long len)
{
    _mem = new char[len];
    _length = len;
}

GLScene::Data::Data(void * d, long len)
{
    _mem = (char *)d;
    _length = len;
}

void * GLScene::Data::data()
{
    return _mem;
}

long GLScene::Data::length()
{
    return _length;
}

void GLScene::Data::setName(const char * name)
{
    if(_dataName) delete _dataName;
    _dataName = new string(name);
}

bool GLScene::Data::operator==(string & name)
{
    return (strcmp(name.c_str(), _dataName->c_str()) == 0);
}

bool GLScene::Data::operator==(const char * name)
{
    return (strcmp(name, _dataName->c_str()) == 0);
}


GLScene::OtherSrc::~OtherSrc()
{
    switch (type) {
        case 1:
            delete _frameFlag;
            break;
        case 2:
            GLTextureManager::manager()->removeCacheTexture(*_texNameFlag);
            delete _texNameFlag;
        default:
            break;
    }
}

GLScene::OtherSrc::OtherSrc(GLTextureFrameSheet * tfs)
{
    type = 1;
    _frameFlag = tfs;
}

GLScene::OtherSrc::OtherSrc(const char * texName)
{
    type = 2;
    _texNameFlag = new string(texName);
}



#pragma mark -
#pragma mark scene

static void Lua_regite_c_func_to_scene(lua_State * L );

GLScene::~GLScene()
{
    this->onDestroyLua();
    
    // remove the cache data.
    for (map<string, Data*> :: iterator it = _sceneDataSrc.begin();
         it != _sceneDataSrc.end();
         it++)
    {
        delete it->second;
    }
    
    
    for (vector<OtherSrc*>::iterator it = _sceneOtherSrc.begin(); it != _sceneOtherSrc.end(); it++)
    {
        delete (*it);
    }
}

GLScene::GLScene()
{
    float w=0, h = 0;
    GLSupport::openGLViewSize(&w, &h);
    this->setSize(w, h);
#ifdef DEBUG
    this->setColor(MakeColor4B(100, 100, 0, 255));// test color
#else
    this->setColor(MakeColor4B(0, 0, 0, 255));// default black color
#endif
    
    _luaState = nullptr;
    _luaString = nullptr;
    _duration = 5.0f;
}

void GLScene::DataReadCallback(DataCtxType type,
                               char name[512],
                               long datalen,
                               char * buff,
                               void * ctx)
{
    GLScene * secne = static_cast<GLScene *>(ctx);
    if (type == FI_PARTICLE_SYSTEM || type == FI_FRAME_SHEET)
    {
        // cache those data.
        Data * d = new Data(buff,datalen);
        d->setName(name);
        
        string dname(name);
        secne->_sceneDataSrc[dname] = d;
        
        LOG("cache data for name:%s, type:%s",name, type ==FI_FRAME_SHEET ? "frame sheet":"particle system");
    }
    else if(type == FI_TEXTURE)
    {
        // create texture and cache in manager.
        TextureInfo info = createGLTextureWithPNGImageDataAPPLE(buff, datalen);
        GLTexture * text = new GLTexture(info);
        GLTextureManager::manager()->cacheTexture(text, name);
        // as mark, when delete will be delete.
        secne->_sceneOtherSrc.push_back(new OtherSrc(name));
        LOG("create texture for name %s",name);
    }
    else
    {
        LOG("Unkown res type");
    }
}


bool GLScene::loadResouceFromFile(const char * file)
{
    // begin failed.
    if(begin_read_file(file) == JJFAILED) return false;
    
    char * buff=NULL;int length=NULL;
    if(read_Lua_sources(&buff, &length) == JJDONE)
    {
        _luaString = new string(buff);
    }
    else
    {
        return false;
    }
    
    // read many items.
    read_data_items(DataReadCallback,this);
    
    // end
    end_read_file();
    
    return true;
}

bool GLScene::loadResouceFromMemory(const char * buff, long length)
{
    GLAssert(0, "还在测试阶段，等一切都稳定下来的时候自然就会实现了。");
    return true;
}

void GLScene::onSceneCreateElements()
{
    this->callLuaMethodNoneReturn("createElements");
}
void GLScene::onSceneMakeDefault()
{
    this->callLuaMethodNoneReturn("makeDefault");
}

void GLScene::onSceneStart()
{
    this->callLuaMethodNoneReturn("onStart");
}

void GLScene::onSceneEnd()
{
    this->callLuaMethodNoneReturn("onEnd");
}


void GLScene::update(float dt)
{
    this->onSceneStepIn(dt);
}

void GLScene::onSceneStepIn(float dt)
{
    this->callLuaMethod("onStepIn",LP(dt));
}


bool GLScene::onInitLua()
{
    if (_luaString == nullptr || _luaString->length() == 0) { return false; }
    
    char * buffer = NULL;
    long length = loadBundleFileContextWithNameAPPLE("ext_lua_func", &buffer);
    
    
    string lua_sources;
    if (length > 0)
    {
        lua_sources = *_luaString + string(buffer);
    }
    else
    {
        lua_sources = *_luaString;
    }
    
    _luaState = luaL_newstate();
    luaL_openlibs(_luaState);
    
    if (_luaState)
    {
        Lua_regite_c_func_to_scene(_luaState);
        lua_Integer addr = (lua_Integer)pointer_2_integer(this);
        lua_pushinteger(_luaState, addr);
        lua_setglobal(_luaState, "self");
        
        if(luaL_dostring(_luaState, lua_sources.c_str()))
        {
            LOG("%s",lua_tostring(_luaState, -1));
            return false;
        }
    }
    else
    {
        LOG("can not alloc lua state, mem failed.");
        return false;
    }
    return true;
}

void GLScene::onDestroyLua()
{
    // 这里我们调用lua 来清除一些资源。
    // ps:json的资源，在delete的时候会被自动删除的。
    //    创建的sub children 也会在这个scene delete的时候被删除完。
    // 唯一剩下的就是纹理数据了。
    // 这里只是先留这么一个借口在这里。
    // 纹理数据还有其它的方法可以删除的。
    this->callLuaMethodNoneReturn("cleanScene");
    
    // close lua
    if(_luaState)
    {
        lua_close(_luaState);
        _luaState = NULL;
    }
}

void GLScene::callLuaMethodNoneReturn(const char * method)
{
    if(_luaState == NULL) return;
    lua_getglobal(_luaState, method);
    lua_call(_luaState, 0, 1);
    lua_pop(_luaState, 1);
}

Value GLScene::callLuaMethod(const char * method,...)
{
    
    if(_luaState == NULL) return Value();
    lua_getglobal(_luaState, method);
    
    int argc = 0;
    //指向变参的指针
    va_list list;
    //使用第一个参数来初使化list指针
    va_start(list, method);
    
    while (true)
    {
        Value * obj = va_arg(list, Value*);
        
        if(obj == nullptr) break;
        
        if (obj->type() == Value::NULLValue)
        {
            lua_pushnil(_luaState);
        }
        else if (obj->type() == Value::CString || obj->type() == Value::String)
        {
            lua_pushstring(_luaState, obj->asCString());
        }
        else if(obj->type() == Value::Bool)
        {
            lua_pushboolean(_luaState, obj->asBool());
        }
        else if (obj->type() == Value::Double)
        {
            lua_pushnumber(_luaState, obj->asDouble());
        }
        else if(obj->type() == Value::Int)
        {
            lua_pushinteger(_luaState, obj->asInt());
        }
        else
        {
            lua_pushnil(_luaState);
        };
        
        // now ,it was useless.
        delete obj;
        
        argc++;
    }
    
    va_end(list);
    
    lua_call(_luaState, argc, 1);
    int type = lua_type(_luaState, -1);
    
    Value ret_obj;
    switch (type)
    {
        case LUA_TNIL:
            return Value((int)lua_tointeger(_luaState, -1));
            break;
        case LUA_TNUMBER:
            return Value(lua_tonumber(_luaState, -1));
            break;
        case LUA_TBOOLEAN:
            return Value(lua_toboolean(_luaState, -1));
            break;
        case LUA_TSTRING:
            return Value(lua_tostring(_luaState, -1));
            break;
        case LUA_TTABLE:
        case LUA_TFUNCTION:
        case LUA_TUSERDATA:
        case LUA_TTHREAD:
        case LUA_TLIGHTUSERDATA:
            LOG("return type not support, implement by your self.");
        default:
            break;
    }
    lua_pop(_luaState, 1);
    
    return ret_obj;

}

#pragma mark -
#pragma mark Lua callback C func
#ifdef __cplusplus
extern "C" {
#endif
    
#define LUA_INT(_idx_) (luaL_checkinteger(L, _idx_))
#define LUA_FLOAT(_idx_) ((double)luaL_checknumber(L,_idx_))
#define CHAR_PTR_FROM_LUA(_idx_) (luaL_checklstring(L, _idx_, NULL))

    
    static int LuaC_print_objc(lua_State * L)
    {
//        NSInteger obj_addr = LUA_INT(1);
//        Log("%s",obj);
        return 0;
    }

    
    
#ifdef __cplusplus
}
#endif



void Lua_regite_c_func_to_scene(lua_State * L )
{
    lua_register(L, "PrintObj", LuaC_print_objc);
}


#pragma mark - 
#pragma mark GLSceneManager


GLSceneManager::~GLSceneManager()
{

}

GLSceneManager::GLSceneManager()
{
    
}

void GLSceneManager::addScene(GLObjectBase * scene)
{
    
}

void GLSceneManager::removeScene(GLObjectBase * scene)
{
    
}

void GLSceneManager::removeSceneAtIndex(int index)
{
    
}

void GLSceneManager::removeAll()
{
    
}
