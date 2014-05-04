
/*precision mediump float;*/

uniform sampler2D s_texture;

varying vec2 v_texCoord;
varying vec4 v_color;
varying float v_alpha;

void main()
{
    /*color and texture blend */
    vec4 pixelColor = texture2D(s_texture, v_texCoord);
    gl_FragColor = pixelColor * v_color * v_alpha;
}
