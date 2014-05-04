
attribute vec4  a_position;
attribute vec4  a_color;// differ
attribute vec2  a_texCoord;


uniform mat4    a_objMatrix;
uniform mat4    a_mvMatrix;// model view matrix
uniform mat4    a_pMatrix; // project matrix

varying vec4    v_color;
varying vec2    v_texCoord;

void main()
{
    v_color = a_color;
    v_texCoord = a_texCoord;
    
    gl_Position = a_pMatrix*a_mvMatrix*a_objMatrix*a_position;
}