#version 410 core

uniform mat4 uViewProjMatrix;
uniform mat4 uModelMatrix;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoord;

out vec3 aPos;
out vec2 aTexCoord;

void main()
{
	vec4 worldPos = uModelMatrix * vec4(inPos, 1.0);

	aTexCoord = inTexCoord;
	aPos = vec3(worldPos);
	gl_Position = uViewProjMatrix * worldPos;
}
