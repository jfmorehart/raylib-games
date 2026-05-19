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

float scale(float toscale, float factor){
    return round(toscale * factor) / factor;
}

void main()                                                                                                                                                                                        
{
    //GET WORLDSPACE UVS
    vec2 screenCoords = vec2(gl_FragCoord.x, gl_FragCoord.y);
    screenCoords -= (resolution * 0.5);
    screenCoords /= (resolution.y * 0.5);
    screenCoords *= worldScale;
    screenCoords += cameraPosition;

    vec2 wPos = screenCoords;
    // wPos += fragTexCoord * 0.04;

    float stime = _Time * 1;

    float baseFreq = 5;

    //layer one
    float n1 = (octaves(130 + (wPos) * baseFreq, 5));
    float hf = octaves(10 + (wPos) * baseFreq * 10 , 1); 

    float lowFreq = octaves(10 + (wPos) * 10 , 1); 
    // lowFreq = mix(1 - fragTexCoord.y, lowFreq, 0.4);
    wPos -= lowFreq * 0.1;


    float noise = (1 - fragTexCoord.y) - 0.1;
    noise -= lowFreq + n1 * 1 + hf * 0.3;

    float mask = step(0.01, noise);//+ sin(_Time + wPos.x * 5) * 0.005);

    float tfac = fract(0.25 + wPos.x); 
    float seamask = step(-0.07 + n1 * 0.01, noise - tfac * 0.05);

    noise += (n1 - 0.5) * 2;
    noise *= 0.3;
    noise = pow(scale(noise, 10) * 2, 2) * 0.5;// * 0.5;

    // noise = mix(1 - fragTexCoord.y, noise, 0.4);
    float rdot = dotsize + noise;

    
    rdot = clamp(rdot, 0.1, 0.5);
    
    vec2 uvContinuous = wPos * multiplier;//(wPos * 0.8 + n1 * 0.001) * multiplier;
    vec2 uvCell = fract(uvContinuous);

    float d = length(uvCell - 0.5);
    float w = length(fwidth(uvContinuous));
    float val = 1 - smoothstep(rdot - w , rdot + w, d);


    float rec = val * 0.7;

    finalColor = vec4(rec, rec, rec, seamask);
}                                                                                                                                                                                                  
