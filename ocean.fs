#version 330

in vec4 fragColor;
out vec4 finalColor;  
uniform vec2 resolution;    
uniform int multiplier = 1;
uniform float dotsize = 1;
uniform vec2 mpos;
uniform float _Time;

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
    return value;
}

void main()                                                                                                                                                                                        
{
    vec3 col = vec3(1, 1, 1);


    float distRaw = distance(mpos, gl_FragCoord.xy);
    float pDist = distRaw / resolution.y;

    pDist += octaves(gl_FragCoord.xy * 0.005, 2) * 0.4;
    pDist = pow(pDist, 0.5);
    pDist = clamp(pDist, 0.2, 0.7);
    pDist = 0.7 - pDist;


    vec2 uv = gl_FragCoord.xy / resolution.x;   
    float stime = sin(_Time * 3);
    uv += octaves(130 + (4 * stime + vec2(gl_FragCoord.x * 0.5, gl_FragCoord.y)) * 0.004, 2) * 0.05;
    uv += octaves(130 + (7 * _Time + stime + vec2(-gl_FragCoord.x, -gl_FragCoord.y * 0.7)) * 0.01, 2) * 0.02;
    vec2 towardsPointer = mpos - gl_FragCoord.xy;
    towardsPointer = (towardsPointer / length(towardsPointer));
    pDist *= 0.05;

    //pDist = pDist *  step(distRaw, 1000);

    uv -= pDist * towardsPointer; 
    uv = fract(uv * multiplier);

    float val = step(length(uv - 0.5), dotsize);
    // val = pDist;
    //val = octaves(gl_FragCoord.xy * 0.01, 10);

    // val = stime;
    col = vec3(val, val, val);

    //col += step(pDist, 300);
    //col += pDist;
    //finalColor = vec4(pDist, pDist, pDist, 1);     
    finalColor = vec4(col.xyz, 1); 
}                                                                                                                                                                                                  
