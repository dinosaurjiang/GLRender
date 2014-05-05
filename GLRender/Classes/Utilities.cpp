//
//  Utilities.h
//  GLRender
//
//  Created by dinosaur on 13-11-29.
//  Copyright (c) 2013  dinosaur. All rights reserved.
//

#include "GLRenderDef.h"
#include "vec2.h"
#include <string.h>
#include "Utilities.h"
#include <stdio.h>
#include <stdlib.h>


unsigned long nextPOTValue(unsigned long x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}



std::string * string_format(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    
    const size_t SIZE = 1024;
    char buffer[SIZE] = { 0 };
    vsnprintf(buffer, SIZE, fmt, ap);
    
    va_end(ap);
    
    return new std::string(buffer);
}


std::string lastPathComponent(std::string & path)
{
    std::string::size_type p = std::string::npos;
#if TARGET_WINDOWS
    p =  path.rfind('\\/');
#else
    p =  path.rfind('/');
#endif
    return std::string(path, p + 1);
}


bool hasPrefix(std::string & str, const std::string & prefix)
{
    std::string::size_type pos = str.find(prefix);
    return pos == 0;
}

bool hasSuffix(std::string & str, const std::string & suffix)
{
    size_t pos = str.rfind(suffix);
    return pos == str.length() - suffix.length();
}


void MyException(const char * kind,
                 const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    
    const size_t SIZE = 1024;
    char buffer[SIZE] = { 0 };
    vsnprintf(buffer, SIZE, fmt, ap);
    
    va_end(ap);
    
    std::string str1(kind);
    std::string str2(buffer);
    RenderException  texception(str1, str2);
    texception.print();
    throw texception;
}


#ifndef TARGET_IOS

//base on http://developer.blackberry.com/native/documentation/core/load_a_texture_with_png_file.html
#include "png.h"
int load_png_texture(const char* filename,
                     int* width, int* height,
                     float* tex_x, float* tex_y,
                     unsigned int *tex)
{
    int i;
    GLuint format;
    //header for testing if it is a png
    png_byte header[8];
    
    if (!tex)
    {
        return EXIT_FAILURE;
    }
    
    //open file as binary
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        return EXIT_FAILURE;
    }
    
    //read the header
    fread(header, 1, 8, fp);
    
    //test if png
    int is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png)
    {
        fclose(fp);
        return EXIT_FAILURE;
    }
    
    
    //create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fclose(fp);
        return EXIT_FAILURE;
    }
    
    //create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
        fclose(fp);
        return EXIT_FAILURE;
    }
    
    //create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return EXIT_FAILURE;
    }
    
    //setup error handling (required without using custom error handlers above)
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return EXIT_FAILURE;
    }
    
    
    png_init_io(png_ptr, fp);
    
    png_set_sig_bytes(png_ptr, 8);
    
    png_read_info(png_ptr, info_ptr);
    
    int bit_depth, color_type;
    png_uint_32 image_width, image_height;
    
    png_get_IHDR(png_ptr, info_ptr, &image_width, &image_height, &bit_depth, &color_type, NULL, NULL, NULL);
    
    
    switch (color_type)
    {
        case PNG_COLOR_TYPE_RGBA:
            format = GL_RGBA;
            break;
        case PNG_COLOR_TYPE_RGB:
            format = GL_RGB;
            break;
        default:
            fprintf(stderr,"Unsupported PNG color type (%d) for texture: %s", (int)color_type, filename);
            fclose(fp);
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            return NULL;
    }
    
    png_read_update_info(png_ptr, info_ptr);
    
    int rowbytes = (int)png_get_rowbytes(png_ptr, info_ptr);
    
    png_byte *image_data = (png_byte*) malloc(sizeof(png_byte) * rowbytes * image_height);
    
    if (!image_data)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return EXIT_FAILURE;
    }
    
    png_bytep *row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * image_height);
    if (!row_pointers)
    {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return EXIT_FAILURE;
    }
    
    // set the individual row_pointers to point at the correct offsets of image_data
    for (i = 0; i < image_height; i++)
    {
        row_pointers[image_height - 1 - i] = image_data + i * rowbytes;
    }
    
    png_read_image(png_ptr, row_pointers);
    
    int tex_width, tex_height;
    
    tex_width = int(nextPOTValue(image_width));
    tex_height = int(nextPOTValue(image_height));
    
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, (*tex));
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    if ((tex_width != image_width) || (tex_height != image_height) )
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, tex_width, tex_height, 0, format, GL_UNSIGNED_BYTE, NULL);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, format, GL_UNSIGNED_BYTE, image_data);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, tex_width, tex_height, 0, format, GL_UNSIGNED_BYTE, image_data);
    }
    GLint err = glGetError();
    
    //clean up memory and close stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    
    if (err == 0) {
        //Return physical width and height of texture if pointers are not null
        if(width)
        {
            *width = image_width;
        }
        
        if (height)
        {
            *height = image_height;
        }
        
        //Return modified texture coordinates if pointers are not null
        if(tex_x)
        {
            *tex_x = ((float) image_width - 0.5f) / ((float)tex_width);
        }
        if(tex_y)
        {
            *tex_y = ((float) image_height - 0.5f) / ((float)tex_height);
        }
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "GL error %i \n", err);
        return EXIT_FAILURE;
    }
}

#endif