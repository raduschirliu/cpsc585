#version 410 core

in vec3 aPos;
in vec4 aColor;
in vec2 aTexCoord;

out vec4 oColor;

uniform sampler2D uMask;
uniform float uTime = 0.0;

const float sinSpeed = -35.0;

float distFromSin(vec2 uv, float a, float b, float c, float d)
{
	float sinX = uv.x * b - c;
	float sinY = a * sin(sinX + uTime * sinSpeed) + d;
	return abs(sinY - uv.y);
}

vec4 getHighlight(vec3 color, float a, float b, float c, float spread)
{
	const float d = 0.5; // Midline
	vec2 uv = aTexCoord;
	float dist = distFromSin(uv, a, b, c, d) * spread;
	float alpha = clamp(1.0 - dist, 0.0, 1.0);
	return vec4(color, alpha);
}

void main()
{
	float maskAlpha = texture(uMask, aTexCoord).r;
	vec4 base = vec4(0.9, 0.05, 0.05, maskAlpha);
		
	//vec3 h1Color = vec3(0.764, 0.403, 0.129);
	vec3 h1Color = vec3(0.8, 0.3, 0.3);
	vec4 h1 = getHighlight(h1Color, 0.4, 1100.0, 0.0, 8.0);
	
	vec3 h2Color = vec3(0.992, 0.968, 0.588);
	//vec3 h2 = getHighlight(h2Color, 0.3, 15.0, 5.0, 20.0);
	
	vec4 color = base + h1;
	
	oColor = color;
}
