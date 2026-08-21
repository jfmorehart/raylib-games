#version 330

in vec4 fragColor;
out vec4 finalColor;  
uniform vec2 resolution;    
uniform int multiplier = 1;
uniform float dotsize = 1;
uniform vec2 mpos;
uniform float _Time;

in vec2 fragTexCoord;

uniform float worldScale;
uniform vec2 cameraPosition;

float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float rand(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	// Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0 - 2.0 * f);
	return (mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y) - 0.4;
}

float octaves(vec2 uv, int octaveCount){
    float value = 0;
    float amplitude = 0.5;
    float frequency = 1;

    for(int i = 0; i < octaveCount; i++){
        value += (rand(uv * frequency) * amplitude);
        amplitude *= 0.5;
        frequency *= 2;
    }
    return value;
}

float scale(float toscale, float factor){
    return round(toscale * factor) / factor;
}

void main()                                                                                                                                                                                        
{
    //GET WORLDSPACE UVS
    vec2 screenCoords = vec2(gl_FragCoord.x, gl_FragCoord.y);
    screenCoords -= (resolution * 0.5);
    screenCoords /= (resolution.y * 0.5);

    vec2 wPos = screenCoords;


    float d2pix = wPos.y;//length(wPos) * length(wPos);


    // wPos.y = 0.01 / d2pix;
    // wPos.y += sin(2 * wPos.x + 1 * wPos.y + _Time * 0.5) * 1;

    // wPos.x /= d2pix;

    // wPos *= 0.01;

    // wPos.y *= 1000;
    // wPos.y = 1 - wPos.y;

    float stime = _Time * 0.05;
    float baseFreq = 20;

    //layer one
    float n1 = (octaves(300 + (wPos.xy  + stime) * baseFreq, 5));
    float n2 = (octaves(200 + (wPos.xy  - stime * vec2(0.3, 0.7)) * baseFreq * 2, 5));
    // n1 -= (wPos.y - 1)* 2;
    float noise = (n1 + n2) / 2;
    noise = pow(scale(noise, 10) * 2, 2) * 2;// * 0.5;

    float rec = noise;
    float alpha = noise * 2 * step(0.2, rec);
    finalColor = vec4(rec,  rec, rec, alpha);
}                                                                                                                                                                                                  
