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

uniform sampler2D texture0;
in vec2 fragTexCoord;

vec3 BoxSample(vec2 uv){
    vec3 col = vec3(0, 0, 0);
    for(int x = -1; x < 2; x++){
        for(int y = -1; y < 2; y++){
            float mult = 1 + step(abs(x), 0.5) * step(abs(y), 0.5) * 3;
            col += texture(texture0, fragTexCoord + vec2(x, y) * 0.001).xyz * mult;
        }
    }
    return col / 9.00;
}
void main(){

    vec3 col;
    col = BoxSample(fragTexCoord).xyz;
    finalColor = vec4(col.xyz, 1);
}