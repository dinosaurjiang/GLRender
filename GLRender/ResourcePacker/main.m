//
//  main.m
//  ResourcePacker
//
//  Created by dinosaur on 13-12-31.
//  Copyright (c) 2013年 dinosaur. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ResFileLoader.h"

#define LUA_CODES_FILE_NAME @"Codes.lua"
/*
 把所有的资源文件放到一个文件夹里面
 然后lua的文件名如上。
 这个程序会打包里面所需的资源到 一个文件里面
 
 命令： xxx | src folder | target file
 */


int main(int argc, const char * argv[])
{
    @autoreleasepool
    {
        if (argc != 3)
        {
            NSLog(@"Usage:%s [input folder] [output file name]",argv[0]);
            exit(0);
        }
        
        NSString * src_folder = [NSString stringWithCString:argv[1] encoding:NSUTF8StringEncoding];
        
        NSString * target_file = [NSString stringWithCString:argv[2] encoding:NSUTF8StringEncoding];
        
        
        [[NSFileManager defaultManager] removeItemAtPath:target_file error:nil];
        
        
        if(begin_write_file([target_file UTF8String]) == JJFAILED) exit(0);
        
        NSData * ldata = [NSData dataWithContentsOfFile:[src_folder stringByAppendingPathComponent:LUA_CODES_FILE_NAME]];
        write_lua_sources([ldata bytes], (unsigned int)[ldata length]);
        
        
        NSMutableArray * buff = [NSMutableArray array];
        NSArray * contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:src_folder error:nil];
        for(NSString * file in contents)
        {
            if ([file isEqualToString:LUA_CODES_FILE_NAME])
                continue;
            
            DataCtxType t = FI_UNKNOW;
            if ([[[file pathExtension] lowercaseString] isEqualToString:@"png"] || [[file lowercaseString] hasSuffix:@".pvr"])
                t = FI_TEXTURE;
            else if([[file lowercaseString] hasSuffix:@"_fs.json"])
                t = FI_FRAME_SHEET;
            else if([[file  lowercaseString] hasSuffix:@"_ps.json"])
                t = FI_PARTICLE_SYSTEM;
            
            if (t == FI_UNKNOW)
            {
                NSLog(@"skip file:%@",file);
                continue;
            }
            
            NSString * full = [src_folder stringByAppendingPathComponent:file];
            NSData * data = [NSData dataWithContentsOfFile:full];
            if(write_data_item([file UTF8String],
                               [data bytes],
                               (unsigned int)[data length],
                               t)==JJFAILED)
            {
                exit(0);
            }
            
            [buff addObject:file];
        }
        
        end_write_file();
        
        printf("======Done=======\n");
        for (NSString * f in buff)
        {
            printf("\"%s\"\n",[f UTF8String]);
        }
    }
    return 0;
}

