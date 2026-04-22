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
	return (mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y) - 0.5;
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
    return value - 0.5;
}

void main()                                                                                                                                                                                        
{
    vec2 screenCoords = vec2(gl_FragCoord.x, gl_FragCoord.y);
    screenCoords -= (resolution * 0.5);
    screenCoords *= worldScale;
    vec2 ns = (screenCoords + cameraPosition * vec2(resolution.y, resolution.y) * 0.5);

    vec3 col;// = vec3(1, 1, 1);

    float distRaw = distance(mpos, gl_FragCoord.xy);
    float pDist = distRaw / (resolution.y * 0.5);

    pDist += octaves(ns * 0.005, 2) * 0.4;
    pDist = pow(pDist, 0.5);
    pDist = clamp(pDist, 0.2, 0.7);
    pDist = 0.7 - pDist;


    vec2 uv = (ns / resolution.y * 0.5);   

    float stime = sin(_Time * 0.005) * 3;
    float ono = octaves(stime * vec2(0.8, 0) * 130 + ns * 0.04, 2) * 0.01;
    float ono2 = 1;//octaves(130 + (8 * _Time + stime + ns) * 0.01, 2) * 0.02;
    
    uv += ono + ono2;

    vec2 towardsPointer = mpos - gl_FragCoord.xy;
    towardsPointer = (towardsPointer / length(towardsPointer));
    pDist *= 0.02;

    //pDist = pDist *  step(distRaw, 1000);

    uv -= pDist * towardsPointer; 
    uv = fract(uv * multiplier);

    float val = step(length(uv - 0.5), dotsize);
    // val = pDist;
    //val = octaves(gl_FragCoord.xy * 0.01, 10);

    val *= 0.5;
    col = vec3(val, val, val) ;
    finalColor = vec4(col.xyz, val); 
}                                                                                                                                                                                                  
