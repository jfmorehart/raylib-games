#version 330

in vec4 fragColor;
out vec4 finalColor;  
uniform vec2 resolution;    
void main()                                                                                                                                                                                        
{
    vec2 uv = gl_FragCoord.xy / resolution;       
    //finalColor = vec4(uv.x, 0 , 0, 1);     
    finalColor = vec4(uv.x, 0, 0, 1);     
}                                                                                                                                                                                                  
