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

    vec2 screenCoords = vec2(gl_FragCoord.x, gl_FragCoord.y);
    screenCoords -= (resolution);
    screenCoords *= worldScale;
    vec2 ns = (screenCoords + cameraPosition * vec2(resolution.y, resolution.y));
    // ns /= resolution.x;
    // ns -= 0.5;
    // ns *= worldScale;
    // ns *= resolution.x;

    vec2 uv = (gl_FragCoord.xy / resolution.x);   

    float stime = _Time * 1;//sin(_Time * 3);
    float baseFreq = 1;
    float n1 = octaves(130 + (ns) * 0.005 * baseFreq, 5);
    // float n2 = (octaves(130 + (vec2(gl_FragCoord.x * 0.5, gl_FragCoord.y)) * 0.015 * baseFreq, 5)) * 1;



    uv = fract(uv * multiplier);
    float noise = n1;// + n2;
    // noise = pow(noise + 0.01, 0.5);
    noise += 0.2;
    noise *= 0.4;

    noise = scale(noise, 8);
    float rdot = dotsize + noise;


    // rdot = 

    rdot = clamp(rdot, 0.03, 0.4);
    
    float val = step(length(uv - 0.5), rdot);
    // val = n1 * 100;
    // val = 0;
    finalColor = vec4(val, val, val, 1); 
}                                                                                                                                                                                                  
