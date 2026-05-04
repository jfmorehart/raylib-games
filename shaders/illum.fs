#version 330

in vec4 fragColor;
out vec4 finalColor;  
uniform vec2 resolution;    
uniform int multiplier = 1;
uniform float dotsize = 1;
uniform vec2 mpos;
uniform float _Time;


uniform float worldScale;
uniform vec2 cameraPosition;
uniform vec3 dotcolor;

void main()                                                                                                                                                                                        
{
    vec2 screenCoords = vec2(gl_FragCoord.x, gl_FragCoord.y);
    screenCoords -= (resolution * 0.5);
    screenCoords /= (resolution.y * 0.5);
    screenCoords *= worldScale;
    screenCoords += cameraPosition;

    vec2 ns = screenCoords;

    vec2 uv = fract(ns * multiplier);
    float val = step(length(uv - 0.5), dotsize);

    vec3 col = dotcolor * val * fragColor.xyz;
    finalColor = vec4(col, val);
}                                                                                                                                                                                                  
