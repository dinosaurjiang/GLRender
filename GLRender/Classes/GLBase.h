//
//  GLBase.h
//  GLRender
//
//  Created by dinosaur on 14-1-3.
//  Copyright (c) 2014 dinosaur. All rights reserved.
//

#ifndef __GLRender__GLBase__
#define __GLRender__GLBase__

#include <iostream>

/*
	所有类的基础类。
	主要功能:声明了类的class。
*/

#define DECLARE_CLASS(_CLASS_) \
virtual const char * className(){ return _CLASS_::Class(); }; \
static const char * Class() { static const char * __clsName = "" # _CLASS_; return __clsName; };


class GLBase
{
public:
    DECLARE_CLASS(GLBase)
    bool isClass(const char *class_str)
    {
        return (strcmp(this->className(),class_str) == 0 ? true:false);
    };
};

#endif /* defined(__GLRender__GLBase__) */
