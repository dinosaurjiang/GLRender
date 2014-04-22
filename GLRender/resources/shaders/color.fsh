
/*precision mediump float;*/

uniform sampler2D s_texture;

varying vec2 v_texCoord;
varying vec4 v_color;
varying float v_alpha;
varying float v_mode;/*default 0:texture, 1 color, 100+ for nothing*/

void main()
{
    if(v_mode < 0.1) /*draw texture.*/
    {
        vec4 pixelColor = texture2D(s_texture, v_texCoord);
        gl_FragColor = pixelColor * v_alpha;
    }
    else if(v_mode > 0.9 && v_mode < 1.1) /*draw color*/
    {
        gl_FragColor = v_color * v_alpha;
    }
    else if(v_mode > 99.9)
    {
        gl_FragColor = vec4(0);
    }
    else
    {
        /*color and texture blend */
        vec4 pixelColor = texture2D(s_texture, v_texCoord);
        gl_FragColor = pixelColor * v_color * v_alpha;
    }
}
