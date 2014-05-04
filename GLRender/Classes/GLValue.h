//
//  GLValue.h
//  GLRender
//
//  Created by dinosaur on 5/4/14.
//  Copyright (c) 2014 dinosaur. All rights reserved.
//

#ifndef __GLRender__GLValue__
#define __GLRender__GLValue__

#include <iostream>


class Value
{
public:
    
    typedef enum ValType
    {
        Unknow,
        NULLValue,
        String,
        CString,
        Double,
        Int,
        Bool
    }ValType;
    
    
    ~Value();
    Value();
    Value(int v);
    Value(double v);
    Value(bool v);
    Value(const char * v);
    Value(std::string & v);
    
    int asInt();
    double asDouble();
    bool asBool();
    std::string * asString();
    const char * asCString();
    void * asNull();
    
    
    int type();
    
private:

    union
    {
        void * cppStringPtr;
        
        int intValue;
        float floatValue;
        double doubleValue;
        bool boolValue;
    }_values;
    
    
    int _type;
};


#endif /* defined(__GLRender__GLValue__) */
