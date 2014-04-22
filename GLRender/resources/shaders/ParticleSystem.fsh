
uniform sampler2D s_texture;

varying vec4    v_color;
varying vec2    v_texCoord;

void main()
{
    vec4 textColor = texture2D(s_texture, v_texCoord);
    gl_FragColor = v_color * textColor;
}
