#version 410 core

in vec3 aPos;
in vec4 aColor;
in vec2 aTexCoord;

out vec4 oColor;

uniform sampler2D uMask;
uniform float uTime;

uniform float uGamma = 2.2;
uniform float uExposure = 1.0;

const float sinSpeed = 35.0;

float distFromSin(vec2 uv, float a, float b, float c, float d)
{
	float sinX = uv.x * b - c;
	float sinY = a * sin(sinX + uTime * sinSpeed) + d;
	return abs(sinY - uv.y);
}

vec3 getHighlight(vec3 color, float a, float b, float c, float spread)
{
	const float d = 0.5; // Midline
	float dist = distFromSin(aTexCoord, a, b, c, d) * spread;
	float alpha = clamp(1.0 - dist, 0.0, 1.0);
	return color * alpha;
}

void main()
{
	float maskAlpha = texture(uMask, aTexCoord).r;
	vec3 base = vec3(0.9, 0.05, 0.05) * maskAlpha;
		
	//vec3 h1Color = vec3(0.764, 0.403, 0.129);
	vec3 h1Color = vec3(0.8, 0.3, 0.3);
	vec3 h1 = getHighlight(h1Color, 0.4, 20.0, 0.0, 8.0);
	
	vec3 h2Color = vec3(0.992, 0.968, 0.588);
	//vec3 h2 = getHighlight(h2Color, 0.3, 15.0, 5.0, 20.0);
	vec3 h2 = vec3(0);
	
	vec3 h3 = vec3(0);
	
	vec3 color = base + h1 + h2 + h3;
	
	// Map from HDR -> LDR
	vec3 mapped = vec3(1.0) - exp(-color * uExposure);
	//mapped = pow(mapped, vec3(1.0 / uGamma));
	
	oColor = vec4(color, 1.0);
}
