#version 410 core

layout (location = 0) in vec3 inPos;

uniform mat4 uLightSpaceMatrix;
uniform mat4 uModelMatrix;

void main()
{
	gl_Position = uLightSpaceMatrix * uModelMatrix * vec4(inPos, 1.0f);
}
