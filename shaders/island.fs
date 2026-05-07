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
    screenCoords *= worldScale;
    screenCoords += cameraPosition;

    vec2 wPos = screenCoords;

    float stime = _Time * 1;
    float baseFreq = 15;

    //layer one
    float n1 = (octaves(130 + (wPos) * baseFreq, 5));
    float n1rescale = (n1 - 0.5) * 2;

    float lowFreq = octaves(10 + (wPos) * 10 , 1); 
    lowFreq = mix(1 - fragTexCoord.y, lowFreq, 0.6);

    float noise = lowFreq + ((n1 - 0.5) * 2) * 0.1;// + n2;

    float mask = step(0.01, noise);//+ sin(_Time + wPos.x * 5) * 0.005);

    float tfac = fract(0.25 * _Time + wPos.x); 
    float seamask = step(-0.07 + n1rescale * 0.01, noise - tfac * 0.05);
    float seamask2 = step(-0.05, noise - tfac * 0.05);

    noise += (n1 - 0.5) * 2;
    noise *= 0.3;
    noise = pow(scale(noise, 10) * 2, 2) * 0.5;// * 0.5;
    float rdot = dotsize + noise;

    rdot = clamp(rdot, 0, 0.5);
    
    vec2 uvContinuous = (wPos * 0.8 + n1 * 0.001) * multiplier;
    vec2 uvCell = fract(uvContinuous);

    float d = length(uvCell - 0.5);
    float w = length(fwidth(uvContinuous));
    float val = 1 - smoothstep(rdot - w , rdot + w, d);
    // val *= noise;//
    // val *= mix(0.6, 1, (n1 * n1) + 0.1);
    // val = pow(val, 0.5) * 0.5;

    float rec = val * noise * mask;

    float sea = (seamask - seamask2) * (0.6 - abs(0.5 - tfac));
    float seasize = 0.1;
    rec += (seamask - seamask2) * 0.7 *  (1 - smoothstep(seasize - w , seasize + w, d));

    rec *= val;
    finalColor = vec4(rec, rec, rec, mask + sea);
}                                                                                                                                                                                                  
