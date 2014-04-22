//
//  xxxx.c
//  PVRTextureLoader
//
//  Created by dinosaur on 13-12-3.
//
//

#if TARGET_IOS

#include <stdio.h>
#include <Foundation/Foundation.h>

#define PVR_TEXTURE_FLAG_TYPE_MASK	0xff
static char PVRTexIdentifier[5] = "PVR!";

enum
{
	kPVRTextureFlagTypePVRTC_2 = 24,
	kPVRTextureFlagTypePVRTC_4
};

typedef struct _PVRTexHeader
{
	uint32_t headerLength;
	uint32_t height;
	uint32_t width;
	uint32_t numMipmaps;
	uint32_t flags;
	uint32_t dataLength;
	uint32_t bpp;
	uint32_t bitmaskRed;
	uint32_t bitmaskGreen;
	uint32_t bitmaskBlue;
	uint32_t bitmaskAlpha;
	uint32_t pvrTag;
	uint32_t numSurfs;
} PVRTexHeader;


bool createGLTextureFromPVRFile(const void * data, long length)
{
    if(data == NULL || length < sizeof(PVRTexHeader))
        return 0;
    
	NSMutableArray *_imageData;
	
	GLuint _name;
	uint32_t _width, _height;
	GLenum _internalFormat;
	bool _hasAlpha;
    
    
    _imageData = [[NSMutableArray alloc] initWithCapacity:10];
    
    _name = 0;
    _width = _height = 0;
    _internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
    _hasAlpha = false;
    
    
    //unpackPVRData
    bool success = false;
    {
        PVRTexHeader *header = NULL;
        uint32_t flags, pvrTag;
        uint32_t dataLength = 0, dataOffset = 0, dataSize = 0;
        uint32_t blockSize = 0, widthBlocks = 0, heightBlocks = 0;
        uint32_t width = 0, height = 0, bpp = 4;
        uint8_t *bytes = NULL;
        uint32_t formatFlags;
        
        header = (PVRTexHeader *)data;
        
        pvrTag = CFSwapInt32LittleToHost(header->pvrTag);
        
        if (PVRTexIdentifier[0] != ((pvrTag >>  0) & 0xff) ||
            PVRTexIdentifier[1] != ((pvrTag >>  8) & 0xff) ||
            PVRTexIdentifier[2] != ((pvrTag >> 16) & 0xff) ||
            PVRTexIdentifier[3] != ((pvrTag >> 24) & 0xff))
        {
            return false;
        }
        
        flags = CFSwapInt32LittleToHost(header->flags);
        formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;
        
        if (formatFlags == kPVRTextureFlagTypePVRTC_4 || formatFlags == kPVRTextureFlagTypePVRTC_2)
        {
            [_imageData removeAllObjects];
            
            if (formatFlags == kPVRTextureFlagTypePVRTC_4)
                _internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
            else if (formatFlags == kPVRTextureFlagTypePVRTC_2)
                _internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
            
            _width = width = CFSwapInt32LittleToHost(header->width);
            _height = height = CFSwapInt32LittleToHost(header->height);
            
            if (CFSwapInt32LittleToHost(header->bitmaskAlpha))
                _hasAlpha = true;
            else
                _hasAlpha = false;
            
            dataLength = CFSwapInt32LittleToHost(header->dataLength);
            
            bytes = ((uint8_t *)data) + sizeof(PVRTexHeader);
            
            // Calculate the data size for each texture level and respect the minimum number of blocks
            while (dataOffset < dataLength)
            {
                if (formatFlags == kPVRTextureFlagTypePVRTC_4)
                {
                    blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                    widthBlocks = width / 4;
                    heightBlocks = height / 4;
                    bpp = 4;
                }
                else
                {
                    blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
                    widthBlocks = width / 8;
                    heightBlocks = height / 4;
                    bpp = 2;
                }
                
                // Clamp to minimum number of blocks
                if (widthBlocks < 2)
                    widthBlocks = 2;
                if (heightBlocks < 2)
                    heightBlocks = 2;
                
                dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
                
                [_imageData addObject:[NSData dataWithBytes:bytes+dataOffset length:dataSize]];
                
                dataOffset += dataSize;
                
                width = MAX(width >> 1, 1);
                height = MAX(height >> 1, 1);
            }
            
            success = true;
        }
        
    }
    
    if (!success)
    {
        return false;
    }
    
    //createGLTexture
    {
        int width = _width;
        int height = _height;
        NSData *data;
        GLenum err;
        
        if ([_imageData count] > 0)
        {
            if (_name != 0)
                glDeleteTextures(1, &_name);
            
            glGenTextures(1, &_name);
            glBindTexture(GL_TEXTURE_2D, _name);
        }
        
        if ([_imageData count] > 1)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        else
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        for (int i=0; i < [_imageData count]; i++)
        {
            data = [_imageData objectAtIndex:i];
#warning "here the data vers need to deal"
            glCompressedTexImage2D(GL_TEXTURE_2D, i, _internalFormat, width, height, 0, [data length], [data bytes]);
            err = glGetError();
            if (err != GL_NO_ERROR)
            {
                printf("Error uploading compressed texture level: %d. glError: 0x%04X", i, err);
                return false;
            }
            
            width = MAX(width >> 1, 1);
            height = MAX(height >> 1, 1);
        }
        
        [_imageData removeAllObjects];
        
        return true;
    }
    return true;
}


#endif