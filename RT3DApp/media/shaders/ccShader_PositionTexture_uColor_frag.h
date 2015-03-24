precision mediump float;

uniform        vec4 u_color;                
                                        
varying vec2 v_texCoord;                
                                        
uniform sampler2D u_texture;            
                                        
void main()                                
{                                        
    gl_FragColor =  texture2D(u_texture, v_texCoord) * u_color;    
}                                        

