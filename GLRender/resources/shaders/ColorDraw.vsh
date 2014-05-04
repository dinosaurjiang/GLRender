

attribute vec4  a_position;
attribute vec4  a_color;

uniform mat4    a_objMatrix;
uniform mat4    a_mvMatrix;/* model view matrix */
uniform mat4    a_pMatrix; /*project matrix*/
uniform float   a_alpha;

varying vec4    v_color;
varying float   v_alpha;


void main()
{
    /* PASS */
	v_color = a_color;
	v_alpha = a_alpha;
    
    gl_Position = a_pMatrix * a_mvMatrix * a_objMatrix * a_position;
}

