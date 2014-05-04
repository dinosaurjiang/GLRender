
/*precision mediump float;*/

uniform sampler2D s_texture;

varying vec2 v_texCoord;
varying float v_alpha;

void main()
{
    vec4 pixelColor = texture2D(s_texture, v_texCoord);
    gl_FragColor = pixelColor * v_alpha;
}
