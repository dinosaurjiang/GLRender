//
//  GLLight.h
//  GLRender
//
//  Created by dinosaur on 14-5-21.
//  Copyright (c) 2014 dinosaur. All rights reserved.
//

#ifndef __GLRender__GLLight__
#define __GLRender__GLLight__

#include "GLBase.h"
#include "vec4.h"
#include "vec3.h"

// help with ...
//https://github.com/antonholmquist/opengl-es-2-0-shaders
//http://blog.csdn.net/silangquan/article/details/10124155
//http://blog.csdn.net/racehorse/article/details/6662540

using namespace std;


class GLMaterial : public GLBase
{
public:
    DECLARE_CLASS(GLMaterial);

    kmVec4 ambient;
    kmVec4 diffuse;
    kmVec4 specular;
    float shininess;
    
    GLMaterial();
    GLMaterial(float vambient, float vdiffuse, float vspecular , float vshininess);
    ~GLMaterial();
};




class GLLight : public GLBase
{
public:
    DECLARE_CLASS(GLLight);
    
    kmVec3 position;
    kmVec4 ambientColor;
    kmVec4 diffuseColor;
    kmVec4 specularColor;
    
    
    GLLight();
    ~GLLight();
};


class GLDirectionalLight : public GLLight
{
public:
    kmVec3 direction;
    
    GLDirectionalLight();
    ~GLDirectionalLight();
};


class GLPointLight : public GLLight
{
public:
    kmVec3 attenuation;// constant, linear, quadratic
    
    GLPointLight();
    ~GLPointLight();
};


#endif /* defined(__GLRender__GLLight__) */
