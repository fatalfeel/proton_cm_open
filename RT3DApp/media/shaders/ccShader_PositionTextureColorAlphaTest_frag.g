varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform sampler2D u_texture;
uniform float u_alpha_value;

void main()
{
    vec4 texColor = texture2D(u_texture, v_texCoord);

    if ( texColor.a <= u_alpha_value )
        discard;

    gl_FragColor = texColor * v_fragmentColor;
}
