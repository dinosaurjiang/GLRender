//
//  DefaultShader.h
//  GLRender
//
//  Created by dinosaur on 13-12-30.
//  Copyright (c) 2013 dinosaur. All rights reserved.
//

#ifndef GLRender_DefaultShader_h
#define GLRender_DefaultShader_h


static const char * defaultVertexShader =""
"attribute vec4  a_position;  \n"
"attribute vec4  a_color;  \n"
"attribute vec2  a_texCoord;  \n"
"\n"
"uniform mat4    a_objMatrix;  \n"
"uniform mat4    a_mvMatrix;  \n"/* model view matrix */
"uniform mat4    a_pMatrix;  \n" /*project matrix*/
"uniform float   a_alpha;  \n"
"uniform float   a_mode;  \n"
""
"varying vec4    v_color;  \n"
"varying float   v_alpha;  \n"
"varying float   v_mode;  \n"/*default 0:texture, 1 color, 100+ nothing*/
"varying vec2    v_texCoord;  \n"
"\n"
"void main()\n"
"{\n"
    /* PASS */
"	v_color = a_color;  \n"
"	v_alpha = a_alpha;  \n"
"    v_mode = a_mode;  \n"
"    v_texCoord = a_texCoord;  \n"
"\n"
"    gl_Position = a_pMatrix * a_mvMatrix * a_objMatrix * a_position;  \n"
"} ";

static const char * defaultFregmentShader = ""
/*precision mediump float;*/
"uniform sampler2D s_texture;  \n"
""
"varying vec2 v_texCoord;  \n"
"varying vec4 v_color;  \n"
"varying float v_alpha;  \n"
"varying float v_mode;  \n"/*default 0:texture, 1 color, 100+ for nothing*/
""
"void main()  \n"
"{  \n"
"    if(v_mode < 0.1)  \n" /*draw texture.*/
"    {  \n"
"        vec4 pixelColor = texture2D(s_texture, v_texCoord);  \n"
"        gl_FragColor = pixelColor * v_alpha;  \n"
"    }  \n"
"    else if(v_mode > 0.9 && v_mode < 1.1)  \n" /*draw color*/
"    {  \n"
"        gl_FragColor = v_color * v_alpha;  \n"
"    }  \n"
"    else if(v_mode > 99.9)  \n"
"    {  \n"
"        gl_FragColor = vec4(0);  \n"
"    }  \n"
"    else  \n"
"    {  \n"
        /*color and texture blend */
"        vec4 pixelColor = texture2D(s_texture, v_texCoord);  \n"
"        gl_FragColor = pixelColor * v_color * v_alpha;  \n"
"    }  \n"
"} ";




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
