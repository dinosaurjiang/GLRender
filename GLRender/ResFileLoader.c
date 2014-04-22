//
//  JJResFileLoader.c
//  Panda
//
//  Created by dinosaur on 13-10-17.
//  Copyright (c) 2013 dinosaur. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ResFileLoader.h"

#define CHECK_MYHANDER(_HANDLE_)\
if (_HANDLE_ == NULL){ JJPrintf("THERE IS NO FILE HANDLE.");return JJFAILED;}

#define CHECK_HANDER() CHECK_MYHANDER(curFileInstance)


static FILE * curFileInstance = NULL;
static ResFile fileHeader;
static long fsize;


int begin_read_file(const char * path)
{
    if (curFileInstance)
    {
        JJPrintf("there is a file is opened, please call end_read_file() first");
        return JJFAILED;
    }
    
    
    curFileInstance = fopen(path, "rb");
    
    if (!curFileInstance)
    {
        JJPrintf("can not open file:%s",path);
        return JJFAILED;
    }
    
    bzero(&fileHeader, sizeof(ResFile));
    fsize = 0;
    
    fseek(curFileInstance, 0, SEEK_SET);
    fseek(curFileInstance, 0,SEEK_END);
    fsize = ftell(curFileInstance);
    JJPrintf("file size:%ld",fsize);
    
    // read file struct
    fseek(curFileInstance, (fsize - sizeof(ResFile)), SEEK_SET);
    fread(&fileHeader, sizeof(ResFile),1, curFileInstance);
    
    // rewind to header
    fseek(curFileInstance, 0, SEEK_SET);
    
    if (fileHeader.totalLength != fsize)
    {
        JJPrintf("file size != recorded \n");
        end_read_file();
        return JJFAILED;
    }
    
    return JJDONE;
}

int read_Lua_sources(char ** buff, int * length)
{
    CHECK_HANDER();
    
    *length = fileHeader.luaScriptUtf8Length;
    *buff = malloc(fileHeader.luaScriptUtf8Length+1);// +1 for end char
    bzero(*buff, fileHeader.luaScriptUtf8Length+1);
    
    if (!(*buff))
    {
        JJPrintf("alloc mem failed");
        return JJFAILED;
    }
    
    fseek(curFileInstance, 0, SEEK_SET);
    size_t ret = fread(*buff, fileHeader.luaScriptUtf8Length, 1, curFileInstance);
    if (ret != 1)
    {
        JJPrintf("read lua src failed:%zd",ret);
        free(*buff);
        *buff = NULL;
        return JJFAILED;
    }
    
    return JJDONE;
}


int read_data_items(ReaderDataCallback callback, void * ctx)
{
    CHECK_HANDER();
    
    // seek to first texture location
    fseek(curFileInstance, fileHeader.luaScriptUtf8Length, SEEK_SET);
    
    
    // skip to next.
    DataCtx tmp;
    DataCtxType type = FI_UNKNOW;
    char * buffer = NULL;
    char itemName[512] = {0};
    long dataLen = 0;
    
    int i = 0;
    for (; i < fileHeader.dataItemCount; i++)
    {
        bzero(&tmp, sizeof(DataCtx));
        fread(&tmp, sizeof(DataCtx), 1, curFileInstance);
        
        if (buffer == NULL)// first in
        {
            buffer = malloc(tmp.dataLength+1);
            if(buffer==NULL)
                JJPrintf("alloc memory failed");
            bzero(buffer, tmp.dataLength);
        }
        else
        {
            // buff is alloc, but too same.
            if (tmp.dataLength > dataLen)
            {
                buffer = realloc(buffer, tmp.dataLength+1);
                if(buffer==NULL)
                    JJPrintf("alloc memory failed");
            }
            
            bzero(buffer, tmp.dataLength);
        }
        // malloc & realloc len+1 for tail '\0'
        
        
        // get name
        bzero(itemName, 512);
        bcopy(tmp.utf8Name, itemName, 512);
        
        // get length
        dataLen = tmp.dataLength;
        
        // get type
        type = tmp.type;
        size_t ret = fread(buffer, tmp.dataLength, 1, curFileInstance);
        
        if (ret == 0) // read data failed.
        {
            JJPrintf("read data from file failed");
        }
        
        if (callback)
        {
            callback(type,itemName,dataLen,buffer,ctx);
        }
        else
        {
            JJPrintf("NO CALLBACK TO RESPONSE.");
        }
    }
    
    if(buffer)
        free(buffer);

    // all readed.
    if(i == fileHeader.dataItemCount)
        return JJDONE;
    
    return JJFAILED;
}


void end_read_file()
{
    if(curFileInstance == NULL) return;
    
    fclose(curFileInstance);
    curFileInstance = 0;
    fsize = 0;
    bzero(&fileHeader, sizeof(ResFile));
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

static FILE * write_handle = NULL;
static int itemCounts = 0;
static int totalLength = 0;
static int luaLength = 0;

int begin_write_file(const char * path)
{
    if (write_handle)
    {
        JJPrintf("there is a write handle opened, please call end Read first\n");
        return JJFAILED;
    }
    
    write_handle = fopen(path, "wb");
    if (write_handle == NULL)
    {
        JJPrintf("open write file failed\n");
        return JJFAILED;
    }
    
    totalLength = 0;
    itemCounts = 0;
    luaLength = 0;
    
    return JJDONE;
}

int write_lua_sources(const char *src, unsigned int length)
{
    CHECK_MYHANDER(write_handle);
    
    fseek(write_handle, 0, SEEK_END);
    size_t ret = fwrite(src, length, 1, write_handle);
    
    if (ret != 1)
    {
        JJPrintf("write lua src failed:%zd\n",ret);
        return JJFAILED;
    }
    
    totalLength += length;
    luaLength = length;
    return JJDONE;
}

int write_data_item(const char * tname,const char * data, unsigned int length, DataCtxType type)
{
    CHECK_MYHANDER(write_handle);
 
    if (luaLength == 0)
    {
        JJPrintf("please write lua src first.\n");
        return JJFAILED;
    }
    
    DataCtx tmp;
    bzero(tmp.utf8Name, 512);
    bcopy(tname, tmp.utf8Name, strlen(tname));
    tmp.dataLength = length;
    tmp.type = type;
    
    fseek(write_handle, 0, SEEK_END);
    size_t ret = fwrite(&tmp, sizeof(DataCtx), 1, write_handle);
    if (ret != 1)
    {
        JJPrintf("write item header failed:%zd\n",ret);
        return JJFAILED;
    }
    
    ret = fwrite(data, length, 1, write_handle);
    if (ret != 1)
    {
        JJPrintf("write item data failed:%zd\n",ret);
        return JJFAILED;
    }
    
    totalLength+= (length + sizeof(DataCtx));
    itemCounts++;
    return JJDONE;
}

void end_write_file()
{
    if (write_handle)
    {
        ResFile writeHeader;
        writeHeader.luaScriptUtf8Length = luaLength;
        writeHeader.dataItemCount = itemCounts;
        writeHeader.totalLength = (totalLength + sizeof(ResFile));
        
        size_t ret = fwrite(&writeHeader, sizeof(ResFile), 1, write_handle);
        if (ret != 1)
        {
            JJPrintf("end file header failed:%zd\n",ret);
        }
        fclose(write_handle);
        
        totalLength = 0;
        itemCounts = 0;
        luaLength = 0;
    }
    else
    {
        JJPrintf("there's no handle to end\n");
    }
}

