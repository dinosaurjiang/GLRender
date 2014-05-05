//
//  GLValue.cpp
//  GLRender
//
//  Created by dinosaur on 5/4/14.
//  Copyright (c) 2014 dinosaur. All rights reserved.
//

#include "GLValue.h"

using namespace std;

Value::~Value()
{
    if (_type == String || _type == CString)
    {
        string * tmp = static_cast<string *>(this->_values.cppStringPtr);
        delete tmp;
        this->_values.cppStringPtr = nullptr;
    }
}

Value::Value()
{
    _type = NULLValue;
}

Value::Value(int v)
{
    _type = Int;
    this->_values.intValue = v;
}

Value::Value(double v)
{
    _type = Double;
    this->_values.doubleValue = v;
}

Value::Value(bool v)
{
    _type = Bool;
    this->_values.boolValue = v;
}

Value::Value(const char * v)
{
    _type = CString;
    if (v != NULL)
    {
        string * tmp = new string(v);
        this->_values.cppStringPtr = tmp;
    }
    else
    {
        this->_values.cppStringPtr = NULL;
    }
}

Value::Value(string & v)
{
    _type = String;
    string * tmp = new string(v);
    this->_values.cppStringPtr = tmp;
}



int Value::asInt()
{
    switch (_type) {
        case Bool:return this->_values.boolValue ? 1 : 0; break;
        case Int: return this->_values.intValue; break;
        case Double: return (int)(this->_values.doubleValue); break;
        case NULLValue:
        case String:
        default:
            break;
    }
    return 0;
}
double Value::asDouble()
{
    switch (_type) {
        case Bool: return this->_values.boolValue ? 1.0 : 0.0; break;
        case Int: return this->_values.intValue * 1.0; break;
        case Double:return this->_values.doubleValue; break;
        case NULLValue:
        case String:
        default:
            break;
    }
    return .0f;
}

bool Value::asBool()
{
    switch (_type) {
        case Bool: return this->_values.boolValue; break;
        case Int: return this->_values.intValue == 0 ? false: true; break;
        case Double:
        case NULLValue:
        case String:
        default:
            break;
    }
    return false;
}

string * Value::asString()
{
    switch (_type)
    {
        case String:
            if(this->_values.cppStringPtr)
                return static_cast<string *>(this->_values.cppStringPtr);
            else
                return nullptr;
            
            break;
        case Bool:
        case Int:
        case Double:
        case NULLValue:
        default:
            break;
    }
    return nullptr;
}

const char * Value::asCString()
{
    if(this->_values.cppStringPtr)
    {
        string * tmp = static_cast<string *>(this->_values.cppStringPtr);
        return tmp->c_str();
    }

    return nullptr;
}

void * Value::asNull()
{
    return nullptr;
}

int Value::type()
{
    return _type;
}


