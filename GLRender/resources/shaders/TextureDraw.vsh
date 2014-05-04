

attribute vec4  a_position;
attribute vec2  a_texCoord;

uniform mat4    a_objMatrix;
uniform mat4    a_mvMatrix;/* model view matrix */
uniform mat4    a_pMatrix; /*project matrix*/
uniform float   a_alpha;

varying float   v_alpha;
varying vec2    v_texCoord;


void main()
{
    /* PASS */
	v_alpha = a_alpha;
    v_texCoord = a_texCoord;
    
    gl_Position = a_pMatrix * a_mvMatrix * a_objMatrix * a_position;
}

