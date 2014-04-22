

attribute vec4  a_position;
attribute vec4  a_color;
attribute vec2  a_texCoord;

uniform mat4    a_objMatrix;
uniform mat4    a_mvMatrix;/* model view matrix */
uniform mat4    a_pMatrix; /*project matrix*/
uniform float   a_alpha;
uniform float   a_mode;

varying vec4    v_color;
varying float   v_alpha;
varying float   v_mode;/*default 0:texture, 1 color, 100+ nothing*/ 
varying vec2    v_texCoord;


void main()
{
    /* PASS */
	v_color = a_color;
	v_alpha = a_alpha;
    v_mode = a_mode;
    v_texCoord = a_texCoord;
    
    gl_Position = a_pMatrix * a_mvMatrix * a_objMatrix * a_position;
}

