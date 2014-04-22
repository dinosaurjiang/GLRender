//
//  GLScene.h
//  GLRender
//
//  Created by dinosaur on 13-12-24.
//  Copyright (c) 2013 dinosaur. All rights reserved.
//

#ifndef __GLRender__GLScene__
#define __GLRender__GLScene__

#include "GLObject.h"
#include "GLTransitions.h"
#include <vector>
#include <map>
#include "ResFileLoader.h"


#ifdef __cplusplus
extern "C" {
#endif
#include <string.h>
#ifdef __cplusplus
}
#endif

#ifndef LUA_STATE
#define LUA_STATE
typedef struct lua_State lua_State;
#endif


using namespace std;

/*
* default back ground color
* size is glview;
*
* also with lua build
*/
class GLTextureFrameSheet;
class LuaParam;
#define LP(v) (new LuaParam(v))

class GLScene : public GLColorLayer
{
    
public:
    
    DECLARE_CLASS(GLScene)
    
    ~GLScene();
    GLScene();
    
    bool loadResouceFromFile(const char * file);
    bool loadResouceFromMemory(const char * buff, long length);
    
    ////////////////////////////////////////////////////////////////
    //  liftcycel of lua.
    //  onInitLua 在new 完的时候调用
    //  onDestroyLua 在delete 的时候调用
    //  init lua 后 onSceneCreateElements。
    //  init lua后  onSceneMakeDefault(可能会反复调用)
    //  onSceneStart 要开始的时候。init 各种动画
    //  onSceneEnd 已经动画结束的时候。清除下资源。
    //  onSceneStepIn 每次update 之前。
    ////////////////////////////////////////////////////////////////
    void onSceneCreateElements();
    void onSceneMakeDefault();
    void onSceneStart();
    void onSceneEnd();
    
    // this method will auto called.
    void onSceneStepIn(float dt);
    
    bool onInitLua();
    void onDestroyLua();
    
    
    
    // 调用lua 函数，没参数，没返回值。
    void callLuaMethodNoneReturn(const char * method);
    
    // 调用lua 函数，有参数(>0个)  一个返回值。
    // 参数请用LP 这个宏。
    LuaParam callLuaMethod(const char * method,...);
    
    // auto call
    virtual void update(float dt);
private:
    
    //  这个类类似于NSData.
    //  但是这个类没那么多功能的。
    //  主要就是把alloc的内存，交给他，
    //  然后当他delete的时候，他也会把交给他的内存free了
    class Data
    {
    public:
        
        ~Data();
        Data(long len);
        Data(void * d, long len);
        void * data();
        long length();
        void setName(const char * name);
        bool operator==(string & name);
        bool operator==(const char * name);
        
    private:
        long        _length = 0;
        char *      _mem = NULL;
        string *    _dataName = nullptr;
    };
    
    //这个类，只是作为一个缓存类，
    //用来mark 自己用过的一些东西的。
    //当scene 被delete的时候。要把自己new的东西删除了。
    class OtherSrc
    {
    public:
        ~OtherSrc();
        OtherSrc(GLTextureFrameSheet * tfs);
        OtherSrc(const char * texName);
    private:
        int type;
        GLTextureFrameSheet *   _frameFlag;
        string *                _texNameFlag;
    };
    
    
    vector<OtherSrc*>       _sceneOtherSrc;
    map<string, Data*>      _sceneDataSrc;
    float                   _duration;
    string*                 _luaString;
    
    lua_State*              _luaState;
    
    
    static void DataReadCallback(DataCtxType type,
                          char name[512],
                          long datalen,
                          char * buff,
                          void * ctx);
};


////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////

typedef enum ValType
{
    VT_Unknow,
    VT_NULL,
    VT_String,
    VT_CString = VT_String,
    VT_Double,
    VT_Int,
    VT_Bool
}ValType;

//TODO: 当拷贝的时候会出错。因为里面有指针。
// test test
class LuaParam
{
public:
    ~LuaParam()
    {
        this->freeMemory();
    }
    
    LuaParam()
    {
        type = VT_NULL;
    }
    
    LuaParam(int v)
    {
        type = VT_Int;
        valuse = malloc(sizeof(int));
        *((int *)valuse) = v;
    }
    
    LuaParam(double v)
    {
        type = VT_Double;
        valuse = malloc(sizeof(double));
        *((double *)valuse) = v;
    }
    
    LuaParam(bool v)
    {
        type = VT_Bool;
        valuse = malloc(sizeof(int));
        *((int *)valuse) = v?1:0;
    }
    
    LuaParam(const char * v)
    {
        type = VT_String;
        valuse = new string(v);
    }
    
    LuaParam(string & v)
    {
        type = VT_String;
        valuse = new string(v);
    }
    
    int asInt()
    {
        switch (type) {
            case VT_Bool:
            case VT_Int:return *((int *)valuse); break;
            case VT_Double:return (*((double *)valuse)); break;
            case VT_NULL:
            case VT_String:
            default:
                break;
        }
        return 0;
    }
    double asDouble()
    {
        switch (type) {
            case VT_Bool:
            case VT_Int:return  (*((int *)valuse)) * 1.0; break;
            case VT_Double:return (*((double *)valuse)); break;
            case VT_NULL:
            case VT_String:
            default:
                break;
        }
        return .0f;
    }
    
    bool asBool()
    {
        switch (type) {
            case VT_Bool:
            case VT_Int:return  (*((int *)valuse))? true : false; break;
            case VT_Double:
            case VT_NULL:
            case VT_String:
            default:
                break;
        }
        return false;
    }
    
    string * asString()
    {
        switch (type)
        {
            case VT_String: return static_cast<string *>(valuse); break;
            case VT_Bool:
            case VT_Int:
            case VT_Double:
            case VT_NULL:
            default:
                break;
        }
        return nullptr;
    }
    const char *asCString()
    {
        return this->asString()->c_str();
    }
    
    void * asNull()
    {
        return nullptr;
    }
    
    ValType vtype(){ return  type; };
    LuaParam & operator=(LuaParam & lp)
    {
        this->freeMemory();
        this->type = lp.type;
        
        switch (this->type) {
            case VT_String: this->valuse = new string(lp.asCString()); break;
            case VT_Bool:
            case VT_Int:
            {
                valuse = malloc(sizeof(int));
                *((int *)valuse) = lp.asInt();
            }
                break;
            case VT_Double:
            {
                valuse = malloc(sizeof(double));
                *((double *)valuse) = lp.asDouble();
            }
                break;
            case VT_NULL:
            default:
                break;
        }
        
        return *this;
    }
private:
    ValType type;
    void * valuse;
    
    void freeMemory()
    {
        if(type == VT_String)
            delete static_cast<string*>(valuse);
        else if(type != VT_NULL)
            free(valuse);
        
        type = VT_Unknow;
        valuse = NULL;
    }
};





////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////

class GLSceneManager
{
    
public:
    ~GLSceneManager();
    GLSceneManager();
    
    void addScene(GLObjectBase * scene);
    void removeScene(GLObjectBase * scene);
    void removeSceneAtIndex(int index);
    void removeAll();// also will delete it
private:
    // glscene and glscenetransform in it
    vector<GLObjectBase *> _sceneStack;
};



#endif /* defined(__GLRender__GLScene__) */
