//
//  DefaultShader.h
//  GLRender
//
//  Created by dinosaur on 13-12-30.
//  Copyright (c) 2013 dinosaur. All rights reserved.
//

#ifndef GLRender_DefaultShader_h
#define GLRender_DefaultShader_h



static const char * defaultParticleVertexShader =""
"attribute vec4  a_position;  \n"
"attribute vec4  a_color;  \n"// differ
"attribute vec2  a_texCoord;  \n"
""
"uniform mat4    a_objMatrix;  \n"
"uniform mat4    a_mvMatrix;  \n"// model view matrix
"uniform mat4    a_pMatrix;  \n" // project matrix
""
"varying vec4    v_color;  \n"
"varying vec2    v_texCoord;  \n"
""
"void main()\n"
"{\n"
"    v_color = a_color;  \n"
"    v_texCoord = a_texCoord;  \n"
"	gl_Position = a_pMatrix*a_mvMatrix*a_objMatrix*a_position;  \n"
"} ";


static const char * defaultParticleFregmentShader =""
"uniform sampler2D s_texture;  \n"
"varying vec4    v_color;  \n"
"varying vec2    v_texCoord;  \n"
"void main()\n"
"{\n"
"    vec4 textColor = texture2D(s_texture, v_texCoord);  \n"
"    gl_FragColor = v_color * textColor;  \n"
"} ";

#endif
