//
//  GLLight.cpp
//  GLRender
//
//  Created by dinosaur on 14-5-21.
//  Copyright (c) 2014 dinosaur. All rights reserved.
//

#include "GLLight.h"

GLMaterial::GLMaterial()
{
    ambient= {0.2, 0.2, 0.2, 1.0};
    diffuse = {0.8, 0.8, 0.8, 1.0};
    specular = {0., 0., 0., 1.};
    shininess = 0.0;
}

GLMaterial::GLMaterial(float vambient,
                       float vdiffuse,
                       float vspecular ,
                       float vshininess)
{
    ambient= {vambient, vambient, vambient, 1.0};
    diffuse = {vdiffuse, vdiffuse, vdiffuse, 1.0};
    specular = {vspecular, vspecular, vspecular, 1.};
    shininess = vshininess;
}
GLMaterial::~GLMaterial(){}




GLLight::GLLight()
{
    ambientColor = {1.0,1.0,1.0,1.0};
    diffuseColor = {1.0,1.0,1.0,1.0};
    specularColor = {1.0,1.0,1.0,1.0};
}
GLLight::~GLLight(){}



GLDirectionalLight::GLDirectionalLight() : GLLight()
{
    direction = {1.0,1.0,1.0};
}
GLDirectionalLight::~GLDirectionalLight(){}



GLPointLight::GLPointLight() : GLLight(){}
GLPointLight::~GLPointLight(){}
