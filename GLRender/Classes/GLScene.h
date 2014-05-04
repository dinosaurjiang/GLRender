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
#include "GLValue.h"

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
* default background color
* size is glview;
*
* also with lua build
*/
class GLTextureFrameSheet;
class LuaParam;
#define LP(v) (new Value(v))

class GLScene : public GLColorLayer
{
    
public:
    
    DECLARE_CLASS(GLScene)
    
    ~GLScene();
    GLScene();
    
    bool loadResouceFromFile(const char * file);
    bool loadResouceFromMemory(const char * buff, long length);
    
    ////////////////////////////////////////////////////////////////
    //  lifecycel of lua.
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
    Value callLuaMethod(const char * method,...);
    
    
    // auto invoked
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
