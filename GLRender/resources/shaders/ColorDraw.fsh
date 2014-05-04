
/*precision mediump float;*/

varying vec4 v_color;
varying float v_alpha;

void main()
{
    gl_FragColor = v_color * v_alpha;
}
