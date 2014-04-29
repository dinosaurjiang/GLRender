//
//  JJResFileLoader.h
//
//  Created by dinosaur on 13-10-17.
//  Copyright (c) 2013 dinosaur. All rights reserved.
//

#ifndef __Dinosaur_ResFileLoader_h__
#define __Dinosaur_ResFileLoader_h__

#ifdef __cplusplus
extern "C" {
#endif
    
    
#pragma mark struct def

typedef enum 
{
    FI_UNKNOW,
    FI_FRAME_SHEET,// text json file data
    FI_TEXTURE,//png file, for ios is pvr.
    FI_PARTICLE_SYSTEM// particle json file data
} DataCtxType;

typedef struct
{
    char utf8Name[512];
    DataCtxType type;
    unsigned int dataLength;
}DataCtx;


typedef struct _resFile
{
    int version; // cur 1. maybe changed.
    unsigned int totalLength;
    unsigned int luaScriptUtf8Length;
    int dataItemCount;
}ResFile;

/* file struct
 *  1.lua resource
 *
 *  2.texture(particle sys) struct.
 *  3.texture(particle sys) data.
 *
 *  .........
 *
 *  ******
 *  repeat by 2,3
 *  ******
 *
 *  at the end
 *      header -- resFile
 */

#pragma mark file read

#define JJDONE 1
#define JJFAILED 0


#ifdef DEBUG
    #define DEBUG_LOG(format,...) printf("File: "__FILE__", Line: %05d: " format "\n", __LINE__, ##__VA_ARGS__)  
#else  
    #define DEBUG_LOG(format,...)  
#endif  

////////////////////////////////////////////////
// callback do not free buff please.
// buff will be reuse.
typedef void (*ReaderDataCallback)(DataCtxType type, char name[512], long datalen, char * buff, void * ctx);


int begin_read_file(const char * path);

// caller responce to free buffer.
int read_Lua_sources(char ** buff, int * length);
int read_data_items(ReaderDataCallback callback, void * ctx);
void end_read_file();

////////////////////////////////////////////////


// not implements
#if 0 
int begin_read_memory(void * address);
// caller responce to free buffer.
int read_Lua_sources_in_memory(char ** buff, int * length);
int read_data_items_in_memory(ReaderDataCallback callback, void * ctx);
void end_read_memory();
#endif


#pragma mark file write

int begin_write_file(const char * path);
int write_lua_sources(const char *src, unsigned int length);
int write_data_item(const char * tname,const char * data, unsigned int length, DataCtxType type);
void end_write_file();
    
#ifdef __cplusplus
}
#endif
        
#endif
